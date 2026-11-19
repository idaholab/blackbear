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

#include "Action.h"

/**
 * Sets up a moving heat source along a SpatioTemporalPath. The path itself is created from the
 * [path] sub-block by AddSpatioTemporalPathAction. This action creates:
 *   - a SpatioTemporalPathElementSubdomainModifier to activate elements along the path
 *   - an ADMovingEllipsoidalHeatSource material
 *   - an ADMatHeatSource kernel
 * All generated objects are named "<block name>_<suffix>" to avoid collisions.
 */
class SpatioTemporalHeatAction : public Action
{
public:
  static InputParameters validParams();

  SpatioTemporalHeatAction(const InputParameters & parameters);

  virtual void act() override;

private:
  /// Name of a generated object, prefixed by this action's name
  std::string objectName(const std::string & suffix) const;
};
