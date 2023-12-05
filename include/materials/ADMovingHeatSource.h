/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "Material.h"
#include "SpatioTemporalPath.h"
#include "SpatioTemporalPathInterface.h"

/**
 * @brief A moving heat source following a SpatioTemporalPath
 */
class ADMovingHeatSource : public Material, public SpatioTemporalPathInterface
{
public:
  static InputParameters validParams();

  ADMovingHeatSource(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;
  virtual ADReal computeHeatSource() = 0;

  /// The path
  const SpatioTemporalPath & _path;

  /// The heat source
  ADMaterialProperty<Real> & _volumetric_heat;

  /// Tangential distance from the heat source
  MaterialProperty<Real> & _tangential_distance;

  /// Normal distance from the heat source
  MaterialProperty<Real> & _normal_distance;
};
