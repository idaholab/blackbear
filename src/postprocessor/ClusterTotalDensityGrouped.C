/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                       BlackBear                              */
/*                                                              */
/*           (c) 2017 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "ClusterTotalDensityGrouped.h"

#include <limits>

registerMooseObject("BlackBearApp", ClusterTotalDensityGrouped);

InputParameters
ClusterTotalDensityGrouped::validParams()
{
  InputParameters params = NodalPostprocessor::validParams();
  params.set<bool>("unique_node_execute") = true;
  MooseEnum group_spacing("linear geometric", "linear");
  params.addClassDescription("Computes the total number density of clusters from a grouped L0/L1 "
                             "cluster dynamics array variable.");
  params.addRequiredCoupledVar("clusters", "Grouped cluster array concentration variable.");
  params.addRequiredParam<unsigned int>(
      "num_cluster_sizes", "Maximum physical cluster size represented by the grouped array.");
  params.addParam<bool>(
      "use_grouping", true, "Whether the coupled array uses grouped-tail storage.");
  params.addParam<unsigned int>(
      "explicit_max", 100, "Maximum physical cluster size stored explicitly before grouped bins.");
  params.addParam<MooseEnum>(
      "group_spacing", group_spacing, "Grouped-tail spacing mode: linear or geometric.");
  params.addParam<unsigned int>(
      "group_linear_width", 25, "Fixed grouped-bin width when group_spacing = linear.");
  params.addParam<Real>("group_geometric_ratio",
                        1.35,
                        "Geometric grouped-bin growth factor when group_spacing = geometric.");
  params.addParam<bool>(
      "enforce_group_nonnegative",
      true,
      "Whether to limit grouped L0/L1 reconstruction to keep bin endpoint concentrations "
      "nonnegative (up to the tolerance factor).");
  params.addParam<Real>(
      "group_nonnegative_tolerance_factor",
      0.0,
      "Relaxation factor for grouped nonnegativity enforcement. Endpoint concentrations are "
      "allowed down to -factor * L0 before limiting.");
  params.addParam<unsigned int>(
      "n_minimum",
      1,
      "Minimum physical cluster size to include in the total density (default: 1, includes "
      "monomers).");
  params.addParam<Real>("atomic_volume", 1.0, "Atomic volume [m^3] for output conversion.");
  return params;
}

ClusterTotalDensityGrouped::ClusterTotalDensityGrouped(const InputParameters & parameters)
  : NodalPostprocessor(parameters),
    _clusters(coupledArrayValue("clusters")),
    _n_minimum(getParam<unsigned int>("n_minimum")),
    _atomic_volume(getParam<Real>("atomic_volume")),
    _layout(getParam<unsigned int>("num_cluster_sizes"),
            getParam<unsigned int>("explicit_max"),
            getParam<bool>("use_grouping"),
            getParam<MooseEnum>("group_spacing").getEnum<ClusterGroupingSpacing>(),
            getParam<unsigned int>("group_linear_width"),
            getParam<Real>("group_geometric_ratio")),
    _enforce_group_nonnegative(getParam<bool>("enforce_group_nonnegative")),
    _group_nonnegative_tolerance_factor(getParam<Real>("group_nonnegative_tolerance_factor")),
    _sum(0.0),
    _count(0.0)
{
  if (_atomic_volume <= 0.0)
    mooseError("ClusterTotalDensityGrouped requires atomic_volume > 0.");
  if (_group_nonnegative_tolerance_factor < 0.0)
    mooseError("ClusterTotalDensityGrouped requires group_nonnegative_tolerance_factor >= 0.");
}

Real
ClusterTotalDensityGrouped::concentrationAt(unsigned int n) const
{
  if (_layout.isExplicitSize(n))
    return _clusters[_qp](_layout.explicitComponent(n));

  const auto & bin = _layout.groupForSize(n);
  const Real l0 = _clusters[_qp](bin.l0_component);
  if (!_enforce_group_nonnegative)
    return l0 + _clusters[_qp](bin.l1_component) * (static_cast<Real>(n) - bin.mean_x);
  if (l0 <= 0.0)
    return 0.0;

  const Real l1 = _clusters[_qp](bin.l1_component);
  const Real left_dx = static_cast<Real>(bin.start) - bin.mean_x;
  const Real right_dx = static_cast<Real>(bin.end) - bin.mean_x;
  const Real allowed_negative = -_group_nonnegative_tolerance_factor * l0;
  Real lower = -std::numeric_limits<Real>::infinity();
  Real upper = std::numeric_limits<Real>::infinity();
  if (right_dx > 0.0)
    lower = (allowed_negative - l0) / right_dx;
  if (left_dx < 0.0)
    upper = (l0 - allowed_negative) / (-left_dx);

  const Real limited_l1 = std::min(std::max(l1, lower), upper);
  return std::max(0.0, l0 + limited_l1 * (static_cast<Real>(n) - bin.mean_x));
}

bool
ClusterTotalDensityGrouped::groupedBinNonnegative(const ClusterGroupingBin & bin) const
{
  if (!_enforce_group_nonnegative)
    return false;

  const Real l0 = _clusters[_qp](bin.l0_component);
  if (l0 <= 0.0)
    return false;

  const Real l1 = _clusters[_qp](bin.l1_component);
  const Real left_dx = static_cast<Real>(bin.start) - bin.mean_x;
  const Real right_dx = static_cast<Real>(bin.end) - bin.mean_x;
  const Real allowed_negative = -_group_nonnegative_tolerance_factor * l0;
  Real lower = -std::numeric_limits<Real>::infinity();
  Real upper = std::numeric_limits<Real>::infinity();
  if (right_dx > 0.0)
    lower = (allowed_negative - l0) / right_dx;
  if (left_dx < 0.0)
    upper = (l0 - allowed_negative) / (-left_dx);

  return l1 >= lower && l1 <= upper;
}

void
ClusterTotalDensityGrouped::initialize()
{
  _sum = 0.0;
  _count = 0.0;
}

void
ClusterTotalDensityGrouped::execute()
{
  Real node_total = 0.0;
  const unsigned int explicit_upper = std::min(_layout.explicitMax(), _layout.numClusterSizes());
  for (unsigned int n = _n_minimum; n <= explicit_upper; ++n)
    node_total += concentrationAt(n);

  for (const auto & bin : _layout.groups())
  {
    if (bin.end < _n_minimum)
      continue;

    if (bin.start >= _n_minimum && groupedBinNonnegative(bin))
    {
      node_total += _clusters[_qp](bin.l0_component) * bin.width;
      continue;
    }

    for (unsigned int n = std::max(_n_minimum, bin.start); n <= bin.end; ++n)
      node_total += concentrationAt(n);
  }

  _sum += node_total;
  _count += 1.0;
}

void
ClusterTotalDensityGrouped::finalize()
{
  gatherSum(_sum);
  gatherSum(_count);
}

void
ClusterTotalDensityGrouped::threadJoin(const UserObject & y)
{
  const auto & rhs = static_cast<const ClusterTotalDensityGrouped &>(y);
  _sum += rhs._sum;
  _count += rhs._count;
}

Real
ClusterTotalDensityGrouped::getValue() const
{
  return _count > 0.0 ? _sum / (_count * _atomic_volume) : 0.0;
}
