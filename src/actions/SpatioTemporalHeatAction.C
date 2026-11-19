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

#include "SpatioTemporalHeatAction.h"
#include "FEProblem.h"
#include "SpatioTemporalPathElementSubdomainModifier.h"
#include "ADMovingEllipsoidalHeatSource.h"

registerMooseAction("BlackBearApp", SpatioTemporalHeatAction, "add_mesh_modifier");
registerMooseAction("BlackBearApp", SpatioTemporalHeatAction, "add_material");
registerMooseAction("BlackBearApp", SpatioTemporalHeatAction, "add_kernel");

namespace
{
/// Subdomain modifier parameters exposed under the same name in the action
const std::vector<std::string> esm_params = {"radius",
                                              "target_subdomain",
                                              "block",
                                              "reinitialize_subdomains",
                                              "old_subdomain_reinitialized",
                                              "reinitialize_variables",
                                              "reinitialization_strategy",
                                              "polynomial_fitters",
                                              "nearby_kd_tree_leaf_max_size",
                                              "nearby_distance_threshold"};

/// Heat source parameters exposed under the same name in the action
const std::vector<std::string> heat_source_params = {"power", "a", "b", "efficiency", "scale"};
}

InputParameters
SpatioTemporalHeatAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription(
      "Sets up a moving heat source along a spatio-temporal path, which is defined in the 'path' "
      "sub-block. Creates the element subdomain modifier, the moving ellipsoidal heat source "
      "material, and the heat source kernel.");

  // The subdomain modifier and heat source parameters are copied from the objects that use them,
  // so their types, defaults, and descriptions stay in sync.
  const auto esm = SpatioTemporalPathElementSubdomainModifier::validParams();
  params.transferParam<Real>(esm, "radius");
  params.transferParam<SubdomainName>(esm, "target_subdomain");
  params.transferParam<std::vector<SubdomainName>>(esm, "block");
  params.transferParam<std::vector<SubdomainName>>(esm, "reinitialize_subdomains");
  params.transferParam<bool>(esm, "old_subdomain_reinitialized");
  params.transferParam<std::vector<VariableName>>(esm, "reinitialize_variables");
  params.transferParam<std::vector<MooseEnum>>(esm, "reinitialization_strategy");
  params.transferParam<std::vector<UserObjectName>>(esm, "polynomial_fitters");
  params.transferParam<int>(esm, "nearby_kd_tree_leaf_max_size");
  params.transferParam<double>(esm, "nearby_distance_threshold");
  params.transferParam<ExecFlagEnum>(
      esm,
      "execute_on",
      "execute_on_esm",
      "When the element subdomain modifier updates the subdomains (its 'execute_on').");

  const auto heat = ADMovingEllipsoidalHeatSource::validParams();
  for (const auto & p : {"power", "a", "b"})
    params.transferParam<MaterialPropertyName>(heat, p);
  params.transferParam<Real>(heat, "efficiency");
  params.transferParam<Real>(heat, "scale");

  params.addRequiredParam<NonlinearVariableName>(
      "heat_variable", "The temperature variable the heat source is applied to");
  return params;
}

SpatioTemporalHeatAction::SpatioTemporalHeatAction(const InputParameters & params)
  : Action(params)
{
  // ElementSubdomainModifier's polynomial reinitialization needs a fitted patch, which does not
  // exist yet at INITIAL, and segfaults there.
  bool polynomial = false;
  for (const auto & strategy : getParam<std::vector<MooseEnum>>("reinitialization_strategy"))
    polynomial |= MooseUtils::startsWith(std::string(strategy), "POLYNOMIAL");
  if (polynomial && getParam<ExecFlagEnum>("execute_on_esm").isValueSet(EXEC_INITIAL))
    paramError("execute_on_esm",
               "INITIAL cannot be combined with a POLYNOMIAL_* 'reinitialization_strategy'.");
}

std::string
SpatioTemporalHeatAction::objectName(const std::string & suffix) const
{
  return name() + "_" + suffix;
}

void
SpatioTemporalHeatAction::act()
{
  // The path built from the [path] sub-block of this action
  const UserObjectName path = objectName("path");

  if (_current_task == "add_mesh_modifier")
  {
    const std::string type = "SpatioTemporalPathElementSubdomainModifier";
    auto p = _factory.getValidParams(type);
    p.applySpecificParameters(parameters(), esm_params);
    p.set<ExecFlagEnum>("execute_on") = getParam<ExecFlagEnum>("execute_on_esm");
    p.set<UserObjectName>("spatiotemporal_path") = path;
    _problem->addUserObject(type, objectName("subdomain_modifier"), p);
  }

  else if (_current_task == "add_material")
  {
    const std::string type = "ADMovingEllipsoidalHeatSource";
    auto p = _factory.getValidParams(type);
    p.applySpecificParameters(parameters(), heat_source_params);
    p.set<UserObjectName>("spatiotemporal_path") = path;
    p.set<MaterialPropertyName>("volumetric_heat") = objectName("volumetric_heat");
    _problem->addMaterial(type, objectName("heat_source"), p);
  }

  else if (_current_task == "add_kernel")
  {
    const std::string type = "ADMatHeatSource";
    auto p = _factory.getValidParams(type);
    p.set<MaterialPropertyName>("material_property") = objectName("volumetric_heat");
    p.set<NonlinearVariableName>("variable") = getParam<NonlinearVariableName>("heat_variable");
    _problem->addKernel(type, objectName("heat_kernel"), p);
  }
}
