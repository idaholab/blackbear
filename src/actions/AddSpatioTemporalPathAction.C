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

registerMooseAction("BlackBearApp", AddSpatioTemporalPathAction, "add_spatiotemporal_path");

InputParameters
AddSpatioTemporalPathAction::validParams()
{
  return MooseObjectAction::validParams();
}

AddSpatioTemporalPathAction::AddSpatioTemporalPathAction(const InputParameters & params)
  : MooseObjectAction(params)
{
  if (!_moose_object_pars.hasBase() || _moose_object_pars.getBase() != "SpatioTemporalPath")
    paramError("type", "'", _type, "' is not a SpatioTemporalPath; only paths can be added here.");
}

void
AddSpatioTemporalPathAction::act()
{
  // [SpatioTemporalPaths/<name>] creates a path called <name>. The [SpatioTemporalHeat/path]
  // sub-block creates "SpatioTemporalHeat_path", the name SpatioTemporalHeatAction uses for it.
  const bool in_heat_action = parameters().blockFullpath() == "SpatioTemporalHeat/path";
  _problem->addObject<SpatioTemporalPath>(
      _type, in_heat_action ? "SpatioTemporalHeat_path" : _name, _moose_object_pars);
}
