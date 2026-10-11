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

#include "GroupedClusterVariableAction.h"

registerMooseAction("BlackBearApp", GroupedClusterVariableAction, "add_variable");

InputParameters
GroupedClusterVariableAction::validParams()
{
  InputParameters params = AddVariableAction::validParams();
  MooseEnum group_spacing("linear geometric", "linear");
  params.addClassDescription(
      "Adds a grouped cluster-dynamics array variable with the component count computed from "
      "the grouped-tail layout.");
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
  return params;
}

GroupedClusterVariableAction::GroupedClusterVariableAction(const InputParameters & params)
  : AddVariableAction(params)
{
}

void
GroupedClusterVariableAction::act()
{
  const ClusterGroupingLayout layout(
      getParam<unsigned int>("num_cluster_sizes"),
      getParam<unsigned int>("explicit_max"),
      getParam<bool>("use_grouping"),
      getParam<MooseEnum>("group_spacing").getEnum<ClusterGroupingSpacing>(),
      getParam<unsigned int>("group_linear_width"),
      getParam<Real>("group_geometric_ratio"));

  _moose_object_pars.set<unsigned int>("components") = layout.componentCount();
  AddVariableAction::act();
}
