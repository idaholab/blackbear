/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "ADMovingHeatSource.h"

InputParameters
ADMovingHeatSource::validParams()
{
  auto params = Material::validParams();
  params.addRequiredParam<std::string>("path",
                                       "The name of the spatio-temporal path object that describes "
                                       "the moving path of the heat source.");
  params.addParam<MaterialPropertyName>(
      "volumetric_heat",
      "volumetric_heat",
      "The name of the material property used to store the computed heat source value.");
  params.addParam<MaterialPropertyName>("heat_source_tangential_distance",
                                        "heat_source_tangential_distance",
                                        "The name of the material property used to store the "
                                        "tangential distance from the heat source.");
  params.addParam<MaterialPropertyName>("heat_source_normal_distance",
                                        "heat_source_normal_distance",
                                        "The name of the material property used to store the "
                                        "normal distance from the heat source.");
  return params;
}

ADMovingHeatSource::ADMovingHeatSource(const InputParameters & params)
  : Material(params),
    SpatioTemporalPathInterface(this),
    _path(getSpatioTemporalPath("path")),
    _volumetric_heat(declareADProperty<Real>("volumetric_heat")),
    _tangential_distance(declareProperty<Real>("heat_source_tangential_distance")),
    _normal_distance(declareProperty<Real>("heat_source_normal_distance"))
{
}

void
ADMovingHeatSource::computeQpProperties()
{
  _tangential_distance[_qp] = _path.tangentialDistance(_q_point[_qp]);
  _normal_distance[_qp] = _path.normalDistance(_q_point[_qp]);
  _volumetric_heat[_qp] = computeHeatSource();
}
