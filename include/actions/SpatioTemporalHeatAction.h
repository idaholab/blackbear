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
 * An Action that sets up a spatiotemporal path heat source simulation.
 * It creates:
 *   - A UserObject for the path
 *   - A MeshModifier to set active/inactive subdomains
 *   - A Material for the heat source
 *   - A Kernel to apply the heat source
 */
class SpatioTemporalHeatAction : public Action
{
public:
  static InputParameters validParams();

  SpatioTemporalHeatAction(const InputParameters & parameters);

  virtual void act() override;
};
