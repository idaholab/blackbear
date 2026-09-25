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

#include "ClusterSizeConcentrationGrouped.h"

registerMooseObject("BlackBearApp", ClusterSizeConcentrationGrouped);

InputParameters
ClusterSizeConcentrationGrouped::validParams()
{
  InputParameters params = NodalPostprocessor::validParams();
  params.set<bool>("unique_node_execute") = true;
  MooseEnum group_spacing("linear geometric", "linear");
  params.addClassDescription("Computes the concentration of one physical cluster size from a "
                             "grouped L0/L1 cluster dynamics array variable.");
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
  params.addParam<unsigned int>("n_size", 1, "Physical cluster size to sample.");
  return params;
}

ClusterSizeConcentrationGrouped::ClusterSizeConcentrationGrouped(const InputParameters & parameters)
  : NodalPostprocessor(parameters),
    _clusters(coupledArrayValue("clusters")),
    _n_size(getParam<unsigned int>("n_size")),
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
  if (!_layout.containsSize(_n_size))
    mooseError("ClusterSizeConcentrationGrouped requested n_size = ",
               _n_size,
               " but the grouped layout only represents sizes up to ",
               _layout.numClusterSizes(),
               ".");
  if (_group_nonnegative_tolerance_factor < 0.0)
    mooseError("ClusterSizeConcentrationGrouped requires group_nonnegative_tolerance_factor >= 0.");
  if (getArrayVar("clusters", 0)->count() != _layout.componentCount())
    paramError("clusters",
               "The grouping layout requires ",
               _layout.componentCount(),
               " array components, but the coupled variable has ",
               getArrayVar("clusters", 0)->count(),
               ". The grouping parameters must match those of the cluster variable.");
}

Real
ClusterSizeConcentrationGrouped::concentrationAt(unsigned int n) const
{
  return _layout.concentration(
      _clusters[_qp], n, _enforce_group_nonnegative, _group_nonnegative_tolerance_factor);
}

void
ClusterSizeConcentrationGrouped::initialize()
{
  _sum = 0.0;
  _count = 0.0;
}

void
ClusterSizeConcentrationGrouped::execute()
{
  _sum += concentrationAt(_n_size);
  _count += 1.0;
}

void
ClusterSizeConcentrationGrouped::finalize()
{
  gatherSum(_sum);
  gatherSum(_count);
}

void
ClusterSizeConcentrationGrouped::threadJoin(const UserObject & y)
{
  const auto & rhs = static_cast<const ClusterSizeConcentrationGrouped &>(y);
  _sum += rhs._sum;
  _count += rhs._count;
}

Real
ClusterSizeConcentrationGrouped::getValue() const
{
  return _count > 0.0 ? _sum / _count : 0.0;
}
