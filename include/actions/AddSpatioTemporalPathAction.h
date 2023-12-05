/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

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
