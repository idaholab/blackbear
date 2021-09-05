//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "AverageExtensionRatio.h"

// MOOSE includes
#include "Function.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "SubProblem.h"

#include "libmesh/system.h"

#include "ComputeIncrementalBeamStrain.h"
#include "Assembly.h"
#include "NonlinearSystem.h"

#include "libmesh/quadrature.h"
#include "libmesh/utility.h"

registerMooseObject("BlackBearApp", AverageExtensionRatio);

defineLegacyParams(AverageExtensionRatio);

InputParameters
AverageExtensionRatio::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<std::vector<VariableName>>(
      "displacements",
      "The displacements appropriate for the simulation geometry and coordinate system");
  params.addRequiredParam<std::vector<Point>>("first_point",
                                              "A list of first points in the numerical domain");
  params.addRequiredParam<std::vector<Point>>("last_point",
                                              "A list of last points in the numerical domain");
  params.addClassDescription(
      "Computes the average extension ratio on deformed mesh between two sets of points");
  return params;
}

AverageExtensionRatio::AverageExtensionRatio(const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    _displacements(getParam<std::vector<VariableName>>("displacements")),

    _system(_subproblem.getSystem(_displacements[0])),
    _first_point(getParam<std::vector<Point>>("first_point")),
    _last_point(getParam<std::vector<Point>>("last_point")),
    _value(0)
{
  _disp_num = std::vector<int>(_displacements.size());
  for (unsigned int i = 0; i < _displacements.size(); ++i)
    _disp_num[i] = _subproblem
                       .getVariable(_tid,
                                    _displacements[i],
                                    Moose::VarKindType::VAR_ANY,
                                    Moose::VarFieldType::VAR_FIELD_STANDARD)
                       .number();

  if (_first_point.size() != _last_point.size() || _first_point.size() == 0)
    mooseError("Sizes of 'first_point' and 'last_point' arrays must match and be postive.");
}

void
AverageExtensionRatio::execute()
{
  _value = 0.;
  Real sq_extension, sq_distance;
  for (unsigned int i = 0; i < _first_point.size(); ++i)
  {
    sq_extension = 0;
    sq_distance = 0;
    for (unsigned int j = 0; j < _displacements.size(); ++j)
    {
      sq_extension += Utility::pow<2>(_system.point_value(_disp_num[j], _first_point[i], false) -
                                      _system.point_value(_disp_num[j], _last_point[i], false));
      sq_distance += Utility::pow<2>(_first_point[i](j) - _last_point[i](j));
    }
    _value += std::sqrt(sq_extension) / std::sqrt(sq_distance);
  }
  _value = _value / _first_point.size();
}

Real
AverageExtensionRatio::getValue()
{
  return _value;
}
