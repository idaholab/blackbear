/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "ADMovingEllipsoidalHeatSource.h"

registerMooseObject("MalamuteApp", ADMovingEllipsoidalHeatSource);

InputParameters
ADMovingEllipsoidalHeatSource::validParams()
{
  auto params = ADMovingHeatSource::validParams();
  params.addClassDescription("A moving ellipsoidal heat source following a SpatioTemporalPath");
  params.addRequiredParam<MaterialPropertyName>("power", "Input power of the heat source.");
  params.addRequiredParam<MaterialPropertyName>(
      "a", "Length of the ellipsoid semi-axis along the path direction");
  params.addRequiredParam<MaterialPropertyName>(
      "b", "Length of the ellipsoid semi-axis perpendicular to the path direction");
  params.addParam<Real>("efficiency", 1.0, "Process efficiency");
  params.addParam<Real>("scale", 1.0, "Scaling factor");
  return params;
}

ADMovingEllipsoidalHeatSource::ADMovingEllipsoidalHeatSource(const InputParameters & params)
  : ADMovingHeatSource(params),
    _P(getADMaterialProperty<Real>("power")),
    _a(getADMaterialProperty<Real>("a")),
    _b(getADMaterialProperty<Real>("b")),
    _eta(getParam<Real>("efficiency")),
    _scale(getParam<Real>("scale"))
{
}

ADReal
ADMovingEllipsoidalHeatSource::computeHeatSource()
{
  auto factor = 2.0 * _P[_qp] * _eta * _scale / libMesh::pi / _a[_qp] / _b[_qp];
  auto dist_t0 = std::sqrt(2.0) * _tangential_distance[_qp] / _a[_qp];
  auto dist_n0 = std::sqrt(2.0) * _normal_distance[_qp] / _b[_qp];

  return factor * std::exp(-dist_t0 * dist_t0 - dist_n0 * dist_n0);
}
