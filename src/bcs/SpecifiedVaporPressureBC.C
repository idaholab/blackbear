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

#include "SpecifiedVaporPressureBC.h"

template<>
InputParameters validParams<SpecifiedVaporPressureBC>()
{
  InputParameters params = validParams<NodalBC>();
  params.set<Real>("duration") = 0.0;
  params.addRequiredParam<Real>("vapor_pressure", "in Pa");
  params.addParam<Real>("T_ref", 20.0, "initial temperature");
  params.addParam<Real>("rh_ref", 0.96, "initial humidity");
  params.addCoupledVar("temperature", "nonlinear variable name holding temperature field");
  return params;
}

SpecifiedVaporPressureBC::SpecifiedVaporPressureBC(const InputParameters & parameters) :
  NodalBC(parameters),
  _duration(getParam<Real>("duration")),
  _vapor_pressure(getParam<Real>("vapor_pressure")),
  _T_ref(getParam<Real>("T_ref")),
  _rh_ref(getParam<Real>("rh_ref")),

  _has_temperature(isCoupled("temperature")),
  _temp(_has_temperature ? coupledValue("temperature") : _zero)
{
}

Real
SpecifiedVaporPressureBC::computeQpResidual()
{
  Real T = _T_ref;

  Real p_sat0 = 101325.0 * std::exp(4871.3 * (_T_ref - 100.0) / 373.15 / (T + 273.15));
  _initial = _rh_ref * p_sat0;
  _final = _vapor_pressure;

  //avoid sudden boundary jump conditions
  Real value;
  if (_t < _duration)
    value = _initial + (_final-_initial)/_duration * _t;
  else
    value = _final;

  if (_has_temperature)
    T = _temp[_qp];

  Real p_sat = 101325.0 * std::exp(4871.3 * (T - 100.0) / 373.15 / (T + 273.15));
  Real rh = value / p_sat;

  return _u[_qp] - rh;
}
