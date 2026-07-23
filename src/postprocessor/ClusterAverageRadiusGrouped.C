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

#include "ClusterAverageRadiusGrouped.h"

#include <cmath>
#include <limits>

registerMooseObject("BlackBearApp", ClusterAverageRadiusGrouped);

InputParameters
ClusterAverageRadiusGrouped::validParams()
{
  InputParameters params = NodalPostprocessor::validParams();
  params.set<bool>("unique_node_execute") = true;
  MooseEnum group_spacing("linear geometric", "linear");
  params.addClassDescription("Computes the number-weighted average cluster radius from a grouped "
                             "L0/L1 cluster dynamics array variable.");
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
  params.addParam<Real>("r1", 1.0, "Monomer radius scale where r_n = r1*n^(1/3)");
  params.addParam<unsigned int>(
      "n_minimum", 2, "Minimum cluster size to include in the average radius.");
  return params;
}

ClusterAverageRadiusGrouped::ClusterAverageRadiusGrouped(const InputParameters & parameters)
  : NodalPostprocessor(parameters),
    _clusters(coupledArrayValue("clusters")),
    _r1(getParam<Real>("r1")),
    _n_minimum(getParam<unsigned int>("n_minimum")),
    _layout(getParam<unsigned int>("num_cluster_sizes"),
            getParam<unsigned int>("explicit_max"),
            getParam<bool>("use_grouping"),
            getParam<MooseEnum>("group_spacing").getEnum<ClusterGroupingSpacing>(),
            getParam<unsigned int>("group_linear_width"),
            getParam<Real>("group_geometric_ratio")),
    _enforce_group_nonnegative(getParam<bool>("enforce_group_nonnegative")),
    _group_nonnegative_tolerance_factor(getParam<Real>("group_nonnegative_tolerance_factor")),
    _radius_weighted_sum(0.0),
    _cluster_sum(0.0)
{
  if (_group_nonnegative_tolerance_factor < 0.0)
    mooseError("ClusterAverageRadiusGrouped requires group_nonnegative_tolerance_factor >= 0.");

  for (const auto & bin : _layout.groups())
  {
    Real radius_sum = 0.0;
    Real radius_centered_sum = 0.0;
    for (unsigned int n = bin.start; n <= bin.end; ++n)
    {
      const Real r_n = _r1 * std::cbrt(static_cast<Real>(n));
      radius_sum += r_n;
      radius_centered_sum += r_n * (static_cast<Real>(n) - bin.mean_x);
    }
    _bin_radius_sum.push_back(radius_sum);
    _bin_radius_centered_sum.push_back(radius_centered_sum);
  }
}

Real
ClusterAverageRadiusGrouped::concentrationAt(unsigned int n) const
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
ClusterAverageRadiusGrouped::groupedBinNonnegative(const ClusterGroupingBin & bin) const
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
ClusterAverageRadiusGrouped::initialize()
{
  _radius_weighted_sum = 0.0;
  _cluster_sum = 0.0;
}

void
ClusterAverageRadiusGrouped::execute()
{
  const unsigned int explicit_upper = std::min(_layout.explicitMax(), _layout.numClusterSizes());
  for (unsigned int n = _n_minimum; n <= explicit_upper; ++n)
  {
    const Real c_n = concentrationAt(n);
    _radius_weighted_sum += _r1 * std::cbrt(static_cast<Real>(n)) * c_n;
    _cluster_sum += c_n;
  }

  const auto & groups = _layout.groups();
  for (unsigned int i = 0; i < groups.size(); ++i)
  {
    const auto & bin = groups[i];
    if (bin.end < _n_minimum)
      continue;

    if (bin.start >= _n_minimum && groupedBinNonnegative(bin))
    {
      const Real l0 = _clusters[_qp](bin.l0_component);
      const Real l1 = _clusters[_qp](bin.l1_component);
      const Real left_dx = static_cast<Real>(bin.start) - bin.mean_x;
      const Real right_dx = static_cast<Real>(bin.end) - bin.mean_x;
      Real lower = -std::numeric_limits<Real>::infinity();
      Real upper = std::numeric_limits<Real>::infinity();
      if (right_dx > 0.0)
        lower = -l0 / right_dx;
      if (left_dx < 0.0)
        upper = l0 / (-left_dx);
      const Real limited_l1 = std::min(std::max(l1, lower), upper);

      _radius_weighted_sum += l0 * _bin_radius_sum[i] + limited_l1 * _bin_radius_centered_sum[i];
      _cluster_sum += l0 * bin.width;
      continue;
    }

    for (unsigned int n = std::max(_n_minimum, bin.start); n <= bin.end; ++n)
    {
      const Real c_n = concentrationAt(n);
      _radius_weighted_sum += _r1 * std::cbrt(static_cast<Real>(n)) * c_n;
      _cluster_sum += c_n;
    }
  }
}

void
ClusterAverageRadiusGrouped::finalize()
{
  gatherSum(_radius_weighted_sum);
  gatherSum(_cluster_sum);
}

void
ClusterAverageRadiusGrouped::threadJoin(const UserObject & y)
{
  const auto & rhs = static_cast<const ClusterAverageRadiusGrouped &>(y);
  _radius_weighted_sum += rhs._radius_weighted_sum;
  _cluster_sum += rhs._cluster_sum;
}

Real
ClusterAverageRadiusGrouped::getValue() const
{
  return _cluster_sum > 0.0 ? _radius_weighted_sum / _cluster_sum : 0.0;
}
