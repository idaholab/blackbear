//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "GroupedArrayFunctionIndexIC.h"

#include "Function.h"

registerMooseObject("BlackBearApp", GroupedArrayFunctionIndexIC);

InputParameters
GroupedArrayFunctionIndexIC::validParams()
{
  InputParameters params = ArrayInitialCondition::validParams();
  MooseEnum group_spacing("linear geometric", "linear");
  params.addRequiredParam<FunctionName>(
      "function",
      "Function evaluated with x equal to the physical cluster size index (0-based) to "
      "initialize the grouped array variable.");
  params.addRequiredParam<unsigned int>(
      "num_cluster_sizes", "Maximum physical cluster size represented by the grouped array.");
  params.addParam<bool>("use_grouping", true, "Whether grouped-tail storage is active.");
  params.addParam<unsigned int>(
      "explicit_max", 100, "Maximum physical cluster size stored explicitly before grouped bins.");
  params.addParam<MooseEnum>(
      "group_spacing", group_spacing, "Grouped-tail spacing mode: linear or geometric.");
  params.addParam<unsigned int>(
      "group_linear_width", 25, "Fixed grouped-bin width when group_spacing = linear.");
  params.addParam<Real>("group_geometric_ratio",
                        1.35,
                        "Geometric grouped-bin growth factor when group_spacing = geometric.");
  params.addClassDescription("Initializes a grouped cluster-dynamics array variable from a "
                             "single function of physical cluster size.");
  return params;
}

GroupedArrayFunctionIndexIC::GroupedArrayFunctionIndexIC(const InputParameters & parameters)
  : ArrayInitialCondition(parameters),
    _function(getFunction("function")),
    _layout(getParam<unsigned int>("num_cluster_sizes"),
            getParam<unsigned int>("explicit_max"),
            getParam<bool>("use_grouping"),
            getParam<MooseEnum>("group_spacing").getEnum<ClusterGroupingSpacing>(),
            getParam<unsigned int>("group_linear_width"),
            getParam<Real>("group_geometric_ratio"))
{
}

RealEigenVector
GroupedArrayFunctionIndexIC::value(const Point & /*p*/)
{
  RealEigenVector values = RealEigenVector::Zero(_var.count());
  if (_var.count() != static_cast<int>(_layout.componentCount()))
    mooseError("GroupedArrayFunctionIndexIC expected ",
               _layout.componentCount(),
               " array components from the grouping layout, but the variable has ",
               _var.count(),
               ".");

  for (unsigned int n = 1; n <= _layout.explicitMax(); ++n)
    values(_layout.explicitComponent(n)) = _function.value(n - 1, Point(n - 1, n - 1, n - 1));

  for (const auto & bin : _layout.groups())
  {
    Real total = 0.0;
    Real first_moment = 0.0;
    for (unsigned int n = bin.start; n <= bin.end; ++n)
    {
      const Real c_n = _function.value(n - 1, Point(n - 1, n - 1, n - 1));
      total += c_n;
      first_moment += static_cast<Real>(n) * c_n;
    }

    const Real l0 = total / bin.width;
    values(bin.l0_component) = l0;
    if (bin.sigma2 > 0.0)
      values(bin.l1_component) = (first_moment / bin.width - l0 * bin.mean_x) / bin.sigma2;
    else
      values(bin.l1_component) = 0.0;
  }

  return values;
}
