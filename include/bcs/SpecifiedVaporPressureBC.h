
#ifndef SPECIFIEDVAPORPRESSUREBC_H
#define SPECIFIEDVAPORPRESSUREBC_H

#include "NodalBC.h"

class SpecifiedVaporPressureBC;

template<>
InputParameters validParams<SpecifiedVaporPressureBC>();

/**
 * Boundary condition of a Dirichlet type
 *
 * Sets the value in the node
 */
class SpecifiedVaporPressureBC : public NodalBC
{
public:
  SpecifiedVaporPressureBC(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpResidual();

  /// The value for this BC
  Real _duration;

  Real _vapor_pressure;
  Real _T_ref;
  Real _rh_ref;


  bool _has_temperature;

  VariableValue & _temp;

  Real _initial;
  Real _final;


};

#endif /* SPECIFIEDVAPORPRESSUREBC_H */
