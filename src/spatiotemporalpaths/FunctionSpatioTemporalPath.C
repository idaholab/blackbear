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

#include "FunctionSpatioTemporalPath.h"
#include "Function.h"

registerMooseObject("BlackBearApp", FunctionSpatioTemporalPath);

InputParameters
FunctionSpatioTemporalPath::validParams()
{
  auto params = SpatioTemporalPath::validParams();
  params.addClassDescription(
      "A spatiotemporal path whose coordinates are specified using MOOSE functions.");
  params.addParam<FunctionName>("x", "The function for the x-coordinate.");
  params.addParam<FunctionName>("y", "The function for the y-coordinate.");
  params.addParam<FunctionName>("z", "The function for the z-coordinate.");
  return params;
}

FunctionSpatioTemporalPath::FunctionSpatioTemporalPath(const InputParameters & params)
  : SpatioTemporalPath(params),
    _x(isParamValid("x") ? &getFunction("x") : nullptr),
    _y(isParamValid("y") ? &getFunction("y") : nullptr),
    _z(isParamValid("z") ? &getFunction("z") : nullptr)
{
}

Point
FunctionSpatioTemporalPath::position(Real t) const
{
  return Point(_x ? _x->value(t) : 0.0, _y ? _y->value(t) : 0.0, _z ? _z->value(t) : 0.0);
}
