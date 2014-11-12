
#include "SpecifiedVaporPressureBC.h"

template<>
InputParameters validParams<SpecifiedVaporPressureBC>()
{
  InputParameters p = validParams<NodalBC>();
  p.set<Real>("duration")=0.0;

  p.addRequiredParam<Real>("vapor_pressure", "in Pa");
  p.addParam<Real>("T_ref",  20.0,  "initial temperature");
  p.addParam<Real>("rh_ref", 0.96, "initial humidity");
  p.addCoupledVar("temperature", "nonlinear variable name holding temperature field");
  return p;
}


SpecifiedVaporPressureBC::SpecifiedVaporPressureBC(const std::string & name, InputParameters parameters) :
  NodalBC(name, parameters),
  _duration(getParam<Real>("duration")),
  _vapor_pressure(getParam<Real>("vapor_pressure")),
  _T_ref(getParam<Real>("T_ref")),
  _rh_ref(getParam<Real>("rh_ref")),

  _has_temperature(isCoupled("temperature")),
  _temp(_has_temperature ? coupledValue("temperature") : _zero)

{}

Real
SpecifiedVaporPressureBC::computeQpResidual()
{

  Real T = _T_ref;

  Real p_sat0 = 101325.0 * std::exp(4871.3*(_T_ref-100)/373.15/(T+273.15));
  _initial = _rh_ref * p_sat0;
  _final = _vapor_pressure;

  //avoid sudden boundary jump conditions

  Real value;
  if (_t < _duration)
    value = _initial + (_final-_initial)/_duration * _t;
  else
    value = _final;

  if (_has_temperature) T = _temp[_qp];

  Real p_sat = 101325.0 * std::exp(4871.3*(T-100)/373.15/(T+273.15));

  Real rh = value / p_sat;

  return _u[_qp] - rh;

}

