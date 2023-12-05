/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "PiecewiseLinearSpatioTemporalPath.h"

registerMooseObject("MalamuteApp", PiecewiseLinearSpatioTemporalPath);

InputParameters
PiecewiseLinearSpatioTemporalPath::validParams()
{
  InputParameters params = PiecewiseLinearSpatioTemporalPathBase::validParams();
  params.addClassDescription(
      "A piecewise linear spatiotemporal path from discrete times and vertices.");
  params.addRequiredParam<std::vector<Real>>("t", "Times associated with the path vertices");
  params.addParam<std::vector<Real>>("x", {}, "x-coordinates of the path vertices");
  params.addParam<std::vector<Real>>("y", {}, "y-coordinates of the path vertices");
  params.addParam<std::vector<Real>>("z", {}, "z-coordinates of the path vertices");
  return params;
}

PiecewiseLinearSpatioTemporalPath::PiecewiseLinearSpatioTemporalPath(const InputParameters & params)
  : PiecewiseLinearSpatioTemporalPathBase(params)
{
  _times = getParam<std::vector<Real>>("t");
  setCoords(getParam<std::vector<Real>>("x"),
            getParam<std::vector<Real>>("y"),
            getParam<std::vector<Real>>("z"));
  validate();
}
