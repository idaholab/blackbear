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

#include "AddSpatioTemporalPathAction.h"
#include "FEProblem.h"
#include "SpatioTemporalPath.h"

registerMooseAction("BlackBearApp", AddSpatioTemporalPathAction, "add_userobject");

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
