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
  /// The list of ids of the primary block (subdomain)
  const std::vector<SubdomainName> _primary;
  /// The list of ids of the secondary block (subdomain)
  const std::vector<SubdomainName> _secondary;
  /// The list of variables on the primary side (defaults to the specified value of 'variable')
  std::vector<VariableName> _primary_var;
  /// Vector of variables for secondary side
  const std::vector<NonlinearVariableName> _variable;
  /// Formulations, currently only supports KINEMATIC and PENALTY
  const enum class Formulation { KINEMATIC, PENALTY } _formulation;
  /// Penalty parameter used in constraint enforcement for kinematic and penalty formulations
  const Real _penalty;
};
