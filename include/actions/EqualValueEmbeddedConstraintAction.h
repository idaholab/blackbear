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
#include "MooseObjectAction.h"
#include "MooseEnum.h"
#include "MooseVariable.h"

class EqualValueEmbeddedConstraintAction : public Action
{
public:
  static InputParameters validParams();

  EqualValueEmbeddedConstraintAction(const InputParameters & params);

  virtual void act();

protected:
  std::vector<SubdomainName> _primary_block;

  std::vector<SubdomainName> _secondary_block;

  /// Vector of displacement variables
  std::vector<NonlinearVariableName> _displacements;

  std::vector<NonlinearVariableName> _primary_var;

  /// Formulations, currently only supports KINEMATIC and PENALTY
  /// Penalty parameter used in constraint enforcement for kinematic and penalty formulations
  const enum class Formulation { KINEMATIC, PENALTY } _formulation;

  /// Penalty parameter used in constraint enforcement for kinematic and penalty formulations
  const Real _penalty;
};
