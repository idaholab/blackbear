/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                        Grizzly                               */
/*                                                              */
/*           (c) 2015 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "ConcreteLatentHeat.h"
#include "Material.h"

template<>
InputParameters validParams<ConcreteLatentHeat>()
{
  InputParameters params = validParams<TimeDerivative>();
  params.addCoupledVar("coupled_vars","coupld to rel. humididty");
  return params;
}

ConcreteLatentHeat::ConcreteLatentHeat(const std::string & name, InputParameters parameters) :
    TimeDerivative(name, parameters),
    _ca(getMaterialProperty<Real>("heat_absorption_of_water")),
    _moisture_capacity(getMaterialProperty<Real>("moisture_capacity"))
{

  int n = coupledComponents("coupled_vars");
  _dvals_dt.resize(n);

  for (unsigned int i=0; i<_dvals_dt.size(); ++i)
  {
    _dvals_dt[i] = &coupledDot("coupled_vars", i);
  }

}

Real
ConcreteLatentHeat::computeQpResidual()
{
  Real _re = 0.0;

//  for (unsigned int i=0; i < _dvals_dt.size(); ++i)
//    _re += _porosity[_qp] * _sto_v[i] * (*_dvals_dt[i])[_qp] * _test[_i][_qp];

  _re = -1.0 * _ca[_qp] * _moisture_capacity[_qp] * (*_dvals_dt[0])[_qp] * _test[_i][_qp];

  return _re;
}

Real
ConcreteLatentHeat::computeQpJacobian()
{
  return 0.0;
}

Real ConcreteLatentHeat::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
