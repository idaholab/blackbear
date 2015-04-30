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

#include "MineralSolutionTimeIntegration.h"
#include "Material.h"

template<>
InputParameters validParams<MineralSolutionTimeIntegration>()
{
  InputParameters params = validParams<TimeDerivative>();
  params.addParam<std::string>("property_name","porosity","The real material property to use");

  params.addCoupledVar("mineral_compositions","mineral Aux variable names involved");
  params.addParam<std::vector<Real> >("sto_v","stochiometric coeff ofreactant species");

  return params;
}

MineralSolutionTimeIntegration::MineralSolutionTimeIntegration(const std::string & name, InputParameters parameters) :
    TimeDerivative(name, parameters),
    _porosity(getMaterialProperty<Real>(getParam<std::string>("property_name"))),
    _sto_v(getParam<std::vector<Real> >("sto_v"))
{

  int n = coupledComponents("mineral_compositions");
  _dvals_dt.resize(n);

  for (unsigned int i=0; i<_dvals_dt.size(); ++i)
  {
    _dvals_dt[i] = &coupledDot("mineral_compositions", i);
  }

}

Real
MineralSolutionTimeIntegration::computeQpResidual()
{
  Real _re = 0.0;

  for (unsigned int i=0; i < _dvals_dt.size(); ++i)
    _re += _porosity[_qp] * _sto_v[i] * (*_dvals_dt[i])[_qp] * _test[_i][_qp];

  return _re;
}

Real
MineralSolutionTimeIntegration::computeQpJacobian()
{
  return 0.0;
}

Real MineralSolutionTimeIntegration::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
