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
#include "Factory.h"
#include "FEProblem.h"
#include "MooseMesh.h"
#include "Material.h"

// Register for different phases of the simulation
registerMooseAction("MooseApp", SpatioTemporalHeatAction, "add_user_object");
registerMooseAction("MooseApp", SpatioTemporalHeatAction, "add_mesh_modifier");
registerMooseAction("MooseApp", SpatioTemporalHeatAction, "add_material");
registerMooseAction("MooseApp", SpatioTemporalHeatAction, "add_kernel");

InputParameters
SpatioTemporalHeatAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addRequiredParam<FileName>("path_file", "CSV file for spatiotemporal path");

  // for CSVPiecewiseLinearSpatioTemporalPath
  params.addParam<bool>("verbose", false, "Verbose path output");

  // for SpatioTemporalPathElementSubdomainModifier
  params.addParam<std::vector<SubdomainName>>(
      "block", {}, "Subdomain blocks for distinguishing active/inactive elements");
  params.addRequiredParam<SubdomainName>(
      "target_subdomain", "The subdomain name/ID to set when the path goes through an element");
  params.addRequiredParam<Real>("radius",
                                "The element subdomain is changed to the target subdomain if its "
                                "centroid is within the radius of the current path front.");

  ExecFlagEnum & exec = params.set<ExecFlagEnum>("execute_on_esm");
  exec.addAvailableFlags(EXEC_INITIAL, EXEC_TIMESTEP_BEGIN, EXEC_TIMESTEP_END, EXEC_FINAL);
  params.setDocString("execute_on_esm", exec.getDocString());

  // for ESM to set the initial condition
  MooseEnum reinit_strategy("IC POLYNOMIAL_NEIGHBOR POLYNOMIAL_WHOLE POLYNOMIAL_NEARBY", "IC");

  params.addParam<std::vector<SubdomainName>>(
      "reinitialize_subdomains",
      {"ANY_BLOCK_ID"},
      "By default, any element which changes subdomain is reinitialized. If a list of subdomains "
      "(IDs or names) is provided, then only elements whose new subdomain is in the list will be "
      "reinitialized. If an empty list is set, then no elements will be reinitialized.");

  params.addParam<bool>(
      "old_subdomain_reinitialized",
      true,
      "This parameter must be set with a non-empty list in 'reinitialize_subdomains'. When set to "
      "the default true, the element's old subdomain is not considered when determining if an "
      "element should be reinitialized. If set to false, only elements whose old subdomain was not "
      "in 'reinitialize_subdomains' are reinitialized. ");

  params.addParam<std::vector<VariableName>>(
      "reinitialize_variables", {}, "Which variables to reinitialize when subdomain changes.");

  params.addParam<std::vector<MooseEnum>>(
      "reinitialization_strategy",
      {reinit_strategy},
      "The strategy used to reinitialize the solution when elements change subdomain. If multiple "
      "strategies are provided, each strategy will be applied to the corresponding variable. If "
      "only one strategy is provided, it will be applied to all variables.");
  params.addParam<std::vector<UserObjectName>>(
      "polynomial_fitters",
      {},
      "List of NodalPatchRecovery UserObjects used to fit the polynomial for reinitialization. "
      "Only needed if 'reinitialization_strategy' is set to POLYNOMIAL_x.");
  params.addParam<int>(
      "nearby_kd_tree_leaf_max_size",
      10,
      "Maximum number of elements in a leaf node of the K-D tree used to search for nearby "
      "elements. Only needed if 'reinitialization_strategy' is set to POLYNOMIAL_NEARBY.");
  params.addParam<double>(
      "nearby_distance_threshold",
      -1.0,
      "Threshold for considering elements as 'nearby' in the K-D tree search. Only elements within "
      "this distance will be considered for polynomial fitting.");

  // for ADMovingEllipsoidalHeatSource
  params.addRequiredParam<MaterialPropertyName>("power", "Input power of the heat source.");
  params.addRequiredParam<MaterialPropertyName>(
      "a", "Length of the ellipsoid semi-axis along the path direction");
  params.addRequiredParam<MaterialPropertyName>(
      "b", "Length of the ellipsoid semi-axis perpendicular to the path direction");
  params.addParam<Real>("efficiency", 1.0, "Process efficiency");
  params.addParam<Real>("scale", 1.0, "Scaling factor");

  // for ADMatHeatSource (kernel)
  params.addRequiredParam<NonlinearVariableName>("heat_variable", "Temperature variable name");

  return params;
}

SpatioTemporalHeatAction::SpatioTemporalHeatAction(const InputParameters & params) : Action(params)
{
}

void
SpatioTemporalHeatAction::act()
{
  const std::string path_name = _name + "path";

  if (_current_task == "add_user_object")
  {
    InputParameters path_params = _factory.getValidParams("CSVPiecewiseLinearSpatioTemporalPath");
    path_params.set<FileName>("file") = getParam<FileName>("path_file");
    path_params.set<bool>("verbose") = getParam<bool>("verbose");

    _problem->addUserObject("CSVPiecewiseLinearSpatioTemporalPath", path_name, path_params);
  }

  else if (_current_task == "add_mesh_modifier")
  {
    InputParameters esm_params =
        _factory.getValidParams("SpatioTemporalPathElementSubdomainModifier");
    esm_params.set<std::string>("path") = path_name;
    esm_params.set<Real>("radius") = getParam<Real>("radius");
    esm_params.set<SubdomainName>("target_subdomain") = getParam<SubdomainName>("target_subdomain");
    esm_params.set<std::vector<SubdomainName>>("block") =
        getParam<std::vector<SubdomainName>>("block");
    esm_params.set<ExecFlagEnum>("execute_on") = getParam<ExecFlagEnum>("execute_on_esm");
    esm_params.set<std::vector<SubdomainName>>("reinitialize_subdomains") =
        getParam<std::vector<SubdomainName>>("reinitialize_subdomains");
    esm_params.set<bool>("old_subdomain_reinitialized") =
        getParam<bool>("old_subdomain_reinitialized");
    esm_params.set<std::vector<VariableName>>("reinitialize_variables") =
        getParam<std::vector<VariableName>>("reinitialize_variables");
    esm_params.set<std::vector<MooseEnum>>("reinitialization_strategy") =
        getParam<std::vector<MooseEnum>>("reinitialization_strategy");
    esm_params.set<std::vector<UserObjectName>>("polynomial_fitters") =
        getParam<std::vector<UserObjectName>>("polynomial_fitters");
    esm_params.set<int>("nearby_kd_tree_leaf_max_size") =
        getParam<int>("nearby_kd_tree_leaf_max_size");
    esm_params.set<double>("nearby_distance_threshold") =
        getParam<double>("nearby_distance_threshold");

    _problem->addUserObject("SpatioTemporalPathElementSubdomainModifier", "esm", esm_params);
  }

  else if (_current_task == "add_material")
  {
    InputParameters mat_params = _factory.getValidParams("ADMovingEllipsoidalHeatSource");
    mat_params.set<std::string>("path") = path_name;
    mat_params.set<MaterialPropertyName>("power") = getParam<MaterialPropertyName>("power");
    mat_params.set<Real>("efficiency") = getParam<Real>("efficiency");
    mat_params.set<Real>("scale") = getParam<Real>("scale");
    mat_params.set<MaterialPropertyName>("a") = getParam<MaterialPropertyName>("a");
    mat_params.set<MaterialPropertyName>("b") = getParam<MaterialPropertyName>("b");

    _problem->addMaterial("ADMovingEllipsoidalHeatSource", "volumetric_heat", mat_params);
  }

  else if (_current_task == "add_kernel")
  {
    InputParameters k_params = _factory.getValidParams("ADMatHeatSource");
    k_params.set<MaterialPropertyName>("material_property") = "volumetric_heat";
    k_params.set<NonlinearVariableName>("variable") =
        getParam<NonlinearVariableName>("heat_variable");

    _problem->addKernel("ADMatHeatSource", "hsource", k_params);
  }
}
