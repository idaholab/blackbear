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

registerMooseObject("BlackBearApp", SpecifiedVaporPressureBC);

InputParameters
SpecifiedVaporPressureBC::validParams()
{
  InputParameters params = NodalBC::validParams();
  params.set<Real>("duration") = 0.0;
  params.addRequiredParam<Real>("vapor_pressure", "Vapor pressure");
  params.setDocUnit("vapor_pressure", "Pa");
  params.addParam<Real>("T_ref", 20.0, "Initial temperature");
  params.setDocUnit("T_ref", "degC");
  params.addParam<Real>("rh_ref", 0.96, "Initial relative humidity");
  params.setDocUnit("rh_ref", "unitless");
  params.addCoupledVar("temperature", "Coupled temperature variable");
  params.setDocUnit("temperature", "degC");
  params.addClassDescription(
      "Prescribed vapor pressure boundary condition for moisture transport in concrete.");
  return params;
}

SpecifiedVaporPressureBC::SpecifiedVaporPressureBC(const InputParameters & parameters)
  : NodalBC(parameters),
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
  Real T_ref_K = _T_ref + 273.15;

  Real p_sat0 = 101325.0 * std::exp(4871.3 * (T_ref_K - 100.0) / 373.15 / T_ref_K);
  _initial = _rh_ref * p_sat0;
  _final = _vapor_pressure;

  // avoid sudden boundary jump conditions
  Real value;
  if (_t < _duration)
    value = _initial + (_final - _initial) / _duration * _t;
  else
    value = _final;

  Real T = T_ref_K;
  if (_has_temperature)
    T = _temp[_qp] + 273.15;

  Real p_sat = 101325.0 * std::exp(4871.3 * (T - 100.0) / 373.15 / T);
  Real rh = value / p_sat;

  return _u[_qp] - rh;
}
