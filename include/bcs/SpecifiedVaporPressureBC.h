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

#include "NodalBC.h"

class SpecifiedVaporPressureBC;

template <>
InputParameters validParams<SpecifiedVaporPressureBC>();

/**
 * Boundary condition of a Dirichlet type
 *
 * Sets the value in the node
 */
class SpecifiedVaporPressureBC : public NodalBC
{
public:
  SpecifiedVaporPressureBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  /// The value for this BC
  Real _duration;

  Real _vapor_pressure;
  Real _T_ref;
  Real _rh_ref;

  bool _has_temperature;

  const VariableValue & _temp;

  Real _initial;
  Real _final;
};
