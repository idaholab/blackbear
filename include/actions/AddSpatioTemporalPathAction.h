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

#include "MooseObjectAction.h"

/**
 * Add SpatioTemporalPath
 */
class AddSpatioTemporalPathAction : public MooseObjectAction
{
public:
  AddSpatioTemporalPathAction(const InputParameters & params);

  static InputParameters validParams();

  void act() override final;
};
