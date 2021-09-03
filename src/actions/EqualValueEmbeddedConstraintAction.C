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
      "primary", "The list of ids of the primary block (subdomain)");
  params.addRequiredParam<std::vector<SubdomainName>>(
      "secondary", "The list of ids of the secondary block (subdomain) ");
  params.addParam<std::vector<VariableName>>(
      "primary_variable",
      "The list of variables on the primary side (defaults to the specified value of 'variable')");
  params.addRequiredParam<std::vector<NonlinearVariableName>>(
      "variable", "The list of variables on the secondary side");
  MooseEnum formulation("kinematic penalty", "kinematic");
  params.addParam<MooseEnum>(
      "formulation", formulation, "Formulation used to enforce the constraint");
  params.addRequiredParam<Real>(
      "penalty",
      "Penalty parameter used in constraint enforcement for kinematic and penalty formulations.");
  return params;
}

EqualValueEmbeddedConstraintAction::EqualValueEmbeddedConstraintAction(
    const InputParameters & params)
  : Action(params),
    _primary(getParam<std::vector<SubdomainName>>("primary")),
    _secondary(getParam<std::vector<SubdomainName>>("secondary")),
    _primary_var(getParam<std::vector<VariableName>>("primary_variable")),
    _variable(getParam<std::vector<NonlinearVariableName>>("variable")),
    _formulation(getParam<MooseEnum>("formulation").getEnum<Formulation>()),
    _penalty(getParam<Real>("penalty"))
{
  if (_primary_var.size() == 0)
    for (unsigned int i = 0; i < _variable.size(); ++i)
      _primary_var.push_back(_variable[i]);
  else if (_primary_var.size() != _variable.size())
    mooseError("Sizes of 'primary_variable' and 'variable' must match.");
}

void
EqualValueEmbeddedConstraintAction::act()
{
  const std::string constraint_name = "EqualValueEmbeddedConstraint";

  SubdomainName pb, sb;
  for (size_t i = 0; i < _variable.size(); i++)
    for (size_t p = 0; p < _primary.size(); p++)
      for (size_t s = 0; s < _secondary.size(); s++)
      {
        std::string unique_constraint_name = constraint_name + "_disp_num_" + Moose::stringify(i) +
                                             "_primary_" + Moose::stringify(p) + "_secondary_" +
                                             Moose::stringify(s);
        InputParameters params = _factory.getValidParams(constraint_name);
        params.applyParameters(parameters(),
                               {"primary", "secondary", "primary_variable", "variable"});
        params.set<SubdomainName>("primary") = _primary[p];
        params.set<SubdomainName>("secondary") = _secondary[s];
        params.set<NonlinearVariableName>("variable") = _variable[i];
        params.set<std::vector<VariableName>>("primary_variable") = {_primary_var[i]};
        params.set<Real>("penalty") = _penalty;
        params.set<MooseEnum>("formulation") = getParam<MooseEnum>("formulation");
        _problem->addConstraint(constraint_name, unique_constraint_name, params);
      }
}
