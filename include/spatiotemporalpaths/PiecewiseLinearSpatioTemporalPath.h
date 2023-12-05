/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "PiecewiseLinearSpatioTemporalPathBase.h"

/**
 * @brief Construct a piecewise linear spatiotemporal path from discrete times and vertices.
 */
class PiecewiseLinearSpatioTemporalPath : public PiecewiseLinearSpatioTemporalPathBase
{
public:
  static InputParameters validParams();

  PiecewiseLinearSpatioTemporalPath(const InputParameters & params);
};
