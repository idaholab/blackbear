/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "SpatioTemporalPath.h"

class Function;

/**
 * @brief A spatiotemporal path whose coordinates are specified using MOOSE functions.
 */
class FunctionSpatioTemporalPath : public SpatioTemporalPath
{
public:
  static InputParameters validParams();

  FunctionSpatioTemporalPath(const InputParameters & params);

  virtual Point position(Real t) const override;

protected:
  /// The function for the x-coordinate
  const Function * _x;
  /// The function for the y-coordinate
  const Function * _y;
  /// The function for the z-coordinate
  const Function * _z;
};
