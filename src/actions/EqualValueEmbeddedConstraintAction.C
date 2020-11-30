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

#include "EqualValueEmbeddedConstraintAction.h"
#include "Conversion.h"
#include "FEProblem.h"
#include "Factory.h"
#include "MooseMesh.h"

#include "libmesh/string_to_enum.h"
// #include <algorithm>

registerMooseAction("BlackBearApp", EqualValueEmbeddedConstraintAction, "add_constraint");

InputParameters
EqualValueEmbeddedConstraintAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription("Sets up constraint on primary and secondary variables on "
                             "primary and secondary block");
  params.addRequiredParam<std::vector<SubdomainName>>(
      "primary_block",
      "The list of ids of the primary block (subdomain)");
  params.addRequiredParam<std::vector<SubdomainName>>(
      "secondary_block",
      "The list of ids of the secondary block (subdomain) ");
  params.addRequiredParam<std::vector<NonlinearVariableName>>(
      "primary_variable",
      "The variable on the primary side of the domain");
  params.addRequiredParam<std::vector<NonlinearVariableName>>(
      "displacements", "The nonlinear displacement variables for the problem");
  MooseEnum formulation("kinematic penalty", "kinematic");
  params.addParam<MooseEnum>(
      "formulation", formulation, "Formulation used to enforce the constraint");
  params.addRequiredParam<Real>(
      "penalty",
      "Penalty parameter used in constraint enforcement for kinematic and penalty formulations.");
  return params;
}

EqualValueEmbeddedConstraintAction::EqualValueEmbeddedConstraintAction(const InputParameters & params)
  : Action(params),
    _primary_block(getParam<std::vector<SubdomainName>>("primary_block")),
    _secondary_block(getParam<std::vector<SubdomainName>>("secondary_block")),
    // _primary_var(*getVar("primary_variable", 0)),
    _primary_var (getParam<std::vector<NonlinearVariableName>>("primary_variable")),
    // _primary_var1((getParam("primary_variable"))[1]),
    // _primary_var2((getParam("primary_variable"))[2]),
    _formulation(getParam<MooseEnum>("formulation").getEnum<Formulation>()),
    _penalty(getParam<Real>("penalty"))

{
  if (!isParamValid("displacements"))
    paramError("displacements",
               "EqualValueEmbeddedConstraintAction: A vector of displacement variable names"
               "should be provided as input using `displacements`.");
  _displacements = getParam<std::vector<NonlinearVariableName>>("displacements");
  // _displacements = getParam<std::vector<NonlinearnonlinearVariableName>>("displacements");
}

void
EqualValueEmbeddedConstraintAction::act()
{
  const std::string constraint_name = "EqualValueEmbeddedConstraint";

  out <<" _displacements " ;

  SubdomainName pb, sb;
  for (size_t i = 0; i < _displacements.size(); i++)
    for (size_t p = 0; p < _primary_block.size(); p++)
      for (size_t s = 0; s < _secondary_block.size(); s++)
      {
        std::string unique_constraint_name = constraint_name +
                                             "_disp_num_"       + Moose::stringify(i) +
                                             "_primaryblock_"   + Moose::stringify(p) +
                                             "_secondaryblock_" + Moose::stringify(s);
        InputParameters params = _factory.getValidParams(constraint_name);
        params.applyParameters(parameters());
        params.set<SubdomainName>("primary") = _primary_block[p];
        params.set<SubdomainName>("secondary") = _secondary_block[s];
        // params.set<std::vector<NonlinearVariableName>>("variable") = {_displacements[i]};
        params.set<NonlinearVariableName>("variable") = _displacements[i];
        params.set<std::vector<NonlinearVariableName>>("primary_variable") = {_primary_var[i]};
        // params.addCoupledVar("primary_variable", "The variable on the primary side of the domain");
        // params.addRequiredCoupledVar("primary_variable", "The variable on the primary side of the domain");

        params.set<Real>("penalty") = _penalty;
        params.set<MooseEnum>("formulation")= getParam<MooseEnum>("formulation");
        _problem->addConstraint(constraint_name, unique_constraint_name, params);

      }
}
