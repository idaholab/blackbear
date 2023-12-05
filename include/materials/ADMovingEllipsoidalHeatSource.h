/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "ADMovingHeatSource.h"

/**
 * @brief A moving ellipsoidal heat source following a SpatioTemporalPath
 */
class ADMovingEllipsoidalHeatSource : public ADMovingHeatSource
{
public:
  static InputParameters validParams();

  ADMovingEllipsoidalHeatSource(const InputParameters & parameters);

protected:
  virtual ADReal computeHeatSource() override;

  /// Input power
  const ADMaterialProperty<Real> & _P;
  /// Length of the ellipsoid semi-axis along the path direction
  const ADMaterialProperty<Real> & _a;
  /// Length of the ellipsoid semi-axis perpendicular to the path direction
  const ADMaterialProperty<Real> & _b;
  /// Process efficienty
  const Real _eta;
  /// Scaling factor
  const Real _scale;
};
