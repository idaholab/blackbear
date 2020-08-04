//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PointsSeparation.h"

// MOOSE includes
#include "Function.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "SubProblem.h"

#include "libmesh/system.h"

registerMooseObject("BlackBearApp", PointsSeparation);

defineLegacyParams(PointsSeparation);

InputParameters
PointsSeparation::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<VariableName>(
      "variable1", "The name of the variable1 that this postprocessor operates on.");
  params.addRequiredParam<VariableName>(
      "variable2", "The name of the variable2 that this postprocessor operates on.");
  params.addRequiredParam<VariableName>(
      "variable3", "The name of the variable3 that this postprocessor operates on.");
  params.addRequiredParam<Point>("point1",
                                 "The physical point 1 where the solution will be evaluated.");
  params.addRequiredParam<Point>("point2",
                                 "The physical point 2 where the solution will be evaluated.");
  params.addClassDescription("Compute the separation between two specified location");
  return params;
}

PointsSeparation::PointsSeparation(const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    _var_number1(_subproblem
                    .getVariable(_tid,
                                 parameters.get<VariableName>("variable1"),
                                 Moose::VarKindType::VAR_ANY,
                                 Moose::VarFieldType::VAR_FIELD_STANDARD)
                    .number()),
    _var_number2(_subproblem
                    .getVariable(_tid,
                                 parameters.get<VariableName>("variable2"),
                                 Moose::VarKindType::VAR_ANY,
                                 Moose::VarFieldType::VAR_FIELD_STANDARD)
                    .number()),
    _var_number3(_subproblem
                    .getVariable(_tid,
                                 parameters.get<VariableName>("variable3"),
                                 Moose::VarKindType::VAR_ANY,
                                 Moose::VarFieldType::VAR_FIELD_STANDARD)
                    .number()),
    _system1(_subproblem.getSystem(getParam<VariableName>("variable1"))),
    _system2(_subproblem.getSystem(getParam<VariableName>("variable2"))),
    _system3(_subproblem.getSystem(getParam<VariableName>("variable3"))),
    _point1(getParam<Point>("point1")),
    _point2(getParam<Point>("point2")),
    _value(0)
{
}

void
PointsSeparation::execute()
{
  Real x1 = _system1.point_value(_var_number1, _point1, false);
  Real y1 = _system2.point_value(_var_number2, _point1, false);
  Real z1 = _system3.point_value(_var_number3, _point1, false);
  Real x2 = _system1.point_value(_var_number1, _point2, false);
  Real y2 = _system2.point_value(_var_number2, _point2, false);
  Real z2 = _system3.point_value(_var_number3, _point2, false);
  _value = std::sqrt(std::pow((x1-x2),2.0) +
                     std::pow((y1-y2),2.0) +
                     std::pow((z1-z2),2.0));
}

Real
PointsSeparation::getValue()
{
  return _value;
}
