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
