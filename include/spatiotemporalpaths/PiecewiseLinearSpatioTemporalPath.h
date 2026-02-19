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
