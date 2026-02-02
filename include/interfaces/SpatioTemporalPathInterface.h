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

#include "MooseTypes.h"
#include "InputParameters.h"

class FEProblemBase;
class InputParameters;
class MooseObject;
class SpatioTemporalPath;

/**
 * Interface for objects that need to use SpatioTemporalPath
 */
class SpatioTemporalPathInterface
{
public:
  SpatioTemporalPathInterface(const MooseObject * moose_object);

  /**
   * Get a SpatioTemporalPath with a given name
   * @param name The name of the parameter key of the SpatioTemporalPath to retrieve
   * @return The SpatioTemporalPath with name associated with the parameter 'name'
   */
  const SpatioTemporalPath & getSpatioTemporalPath(const std::string & name) const;

  /**
   * Get a SpatioTemporalPath with a given name
   * @param name The name of the SpatioTemporalPath to retrieve
   * @return The SpatioTemporalPath with name 'name'
   */
  const SpatioTemporalPath & getSpatioTemporalPathByName(const std::string & name) const;

private:
  /// Parameters of the object with this interface
  const InputParameters & _stpi_params;

  /// Reference to FEProblemBase instance
  FEProblemBase & _stpi_feproblem;

  /// Thread ID
  const THREAD_ID _stpi_tid;
};
