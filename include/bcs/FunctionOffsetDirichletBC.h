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

#include "DirichletBCBase.h"

class Function;

/**
 * Defines a boundary condition that forces the value to be a user specified
 * function at user defined offset distance in the nodal normal direction
 * from the boundary.
 */
class FunctionOffsetDirichletBC : public DirichletBCBase
{
public:
  static InputParameters validParams();

  FunctionOffsetDirichletBC(const InputParameters & parameters);

protected:
  virtual Real computeQpValue() override;

  /// The function being used for evaluation
  const Function & _func;

  const VariableValue & _nx;
  const VariableValue & _ny;
  const VariableValue & _nz;

  const Real _offset;

  /// Vector of the nodal normal
  Point _nor;
};
