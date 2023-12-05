/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "SpatioTemporalPathInterface.h"
#include "SubProblem.h"
#include "MooseTypes.h"
#include "FEProblemBase.h"
#include "SpatioTemporalPath.h"

SpatioTemporalPathInterface::SpatioTemporalPathInterface(const MooseObject * moose_object)
  : _stpi_params(moose_object->parameters()),
    _stpi_feproblem(*_stpi_params.getCheckedPointerParam<FEProblemBase *>("_fe_problem_base")),
    _stpi_tid(_stpi_params.have_parameter<THREAD_ID>("_tid") ? _stpi_params.get<THREAD_ID>("_tid")
                                                             : 0)
{
}

const SpatioTemporalPath &
SpatioTemporalPathInterface::getSpatioTemporalPath(const std::string & param_name) const
{
  return getSpatioTemporalPathByName(_stpi_params.get<std::string>(param_name));
}

const SpatioTemporalPath &
SpatioTemporalPathInterface::getSpatioTemporalPathByName(const std::string & name) const
{
  std::vector<SpatioTemporalPath *> objs;
  return _stpi_feproblem.getUserObject<SpatioTemporalPath>(name, _stpi_tid);
}
