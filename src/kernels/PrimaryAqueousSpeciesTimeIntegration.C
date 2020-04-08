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

#include "PrimaryAqueousSpeciesTimeIntegration.h"
#include "Material.h"

registerMooseObject("BlackBearApp", PrimaryAqueousSpeciesTimeIntegration);

InputParameters
PrimaryAqueousSpeciesTimeIntegration::validParams()
{
  InputParameters params = TimeDerivative::validParams();
  params.addParam<MaterialPropertyName>("property_name", "porosity", "Porosity material property");
  // params.addCoupledVar("mineral_compositions", "mineral Aux variable names involved");
  // params.addParam<std::vector<Real> >("sto_v", "stochiometric coeff ofreactant species");

  return params;
}

PrimaryAqueousSpeciesTimeIntegration::PrimaryAqueousSpeciesTimeIntegration(
    const InputParameters & parameters)
  : TimeDerivative(parameters), _porosity(getMaterialProperty<Real>("property_name"))
// _sto_v(getParam<std::vector<Real> >("sto_v"))
{
  // int n = coupledComponents("mineral_compositions");
  // _dvals_dt.resize(n);
  //
  // for (unsigned int i=0; i<_dvals_dt.size(); ++i)
  // {
  //   _dvals_dt[i] = &coupledDot("mineral_compositions", i);
  // }
}

Real
PrimaryAqueousSpeciesTimeIntegration::computeQpResidual()
{
  Real re = _porosity[_qp] * TimeDerivative::computeQpResidual(); // self accumulation term

  // for (unsigned int i=0; i < _dvals_dt.size(); ++i)
  //   _re += _porosity[_qp] * _sto_v[i] * (*_dvals_dt[i])[_qp] * _test[_i][_qp];
  return re;
}

Real
PrimaryAqueousSpeciesTimeIntegration::computeQpJacobian()
{
  return _porosity[_qp] * TimeDerivative::computeQpJacobian();
}

Real
PrimaryAqueousSpeciesTimeIntegration::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
