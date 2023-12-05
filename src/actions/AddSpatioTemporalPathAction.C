/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "AddSpatioTemporalPathAction.h"
#include "FEProblem.h"
#include "SpatioTemporalPath.h"

registerMooseAction("MalamuteApp", AddSpatioTemporalPathAction, "add_userobject");

InputParameters
AddSpatioTemporalPathAction::validParams()
{
  return MooseObjectAction::validParams();
}

AddSpatioTemporalPathAction::AddSpatioTemporalPathAction(const InputParameters & params)
  : MooseObjectAction(params)
{
}

void
AddSpatioTemporalPathAction::act()
{
  _problem->addObject<SpatioTemporalPath>(_type, _name, _moose_object_pars);
}
