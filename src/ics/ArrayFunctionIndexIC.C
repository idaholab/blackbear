//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ArrayFunctionIndexIC.h"

#include "Function.h"

registerMooseObject("BlackBearApp", ArrayFunctionIndexIC);

InputParameters
ArrayFunctionIndexIC::validParams()
{
  InputParameters params = ArrayInitialCondition::validParams();
  params.addRequiredParam<FunctionName>(
      "function",
      "Function evaluated with x equal to the array component index (0-based) to initialize "
      "each array component value.");
  params.addClassDescription("Initializes an array variable from a single function of component "
                             "index.");
  return params;
}

ArrayFunctionIndexIC::ArrayFunctionIndexIC(const InputParameters & parameters)
  : ArrayInitialCondition(parameters), _function(getFunction("function"))
{
}

RealEigenVector
ArrayFunctionIndexIC::value(const Point & /*p*/)
{
  RealEigenVector values(_var.count());
  for (const auto i : make_range(_var.count()))
    values(i) = _function.value(i, Point(i, i, i));

  return values;
}
