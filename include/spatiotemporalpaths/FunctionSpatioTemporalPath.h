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
