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

#include "Material.h"
#include "ConcreteThermalConvection.h"

registerMooseObject("BlackBearApp", ConcreteThermalConvection);

InputParameters
ConcreteThermalConvection::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addRequiredCoupledVar("relative_humidity", "nonlinear variable name for rel. humidity");
  return params;
}

ConcreteThermalConvection::ConcreteThermalConvection(const InputParameters & parameters)
  : Kernel(parameters),
    _cw(getMaterialProperty<Real>("thermal_capacity_of_water")),
    _Dh(getMaterialProperty<Real>("humidity_diffusivity")),
    _grad_rh(coupledGradient("relative_humidity")),
    _rh_var(coupled("relative_humidity"))
{
}

Real
ConcreteThermalConvection::computeQpResidual()
{
  return _cw[_qp] * _Dh[_qp] * _grad_rh[_qp] * _test[_i][_qp] * _grad_u[_qp];
  // return -_specific_heat_water[_qp]*_darcy_mass_flux_water[_qp]*_grad_test[_i][_qp]*_u[_qp];
}

Real
ConcreteThermalConvection::computeQpJacobian()
{
  return _cw[_qp] * _Dh[_qp] * _grad_rh[_qp] * _test[_i][_qp] * _grad_phi[_j][_qp];
  // return
  // -_specific_heat_water[_qp]*_darcy_mass_flux_water[_qp]*_grad_test[_i][_qp]*_phi[_j][_qp];
}

Real
ConcreteThermalConvection::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _rh_var)
    return _cw[_qp] * _Dh[_qp] * _grad_u[_qp] * _test[_i][_qp] * _grad_phi[_j][_qp];

  return 0.0;
  // return
  // -_specific_heat_water[_qp]*_darcy_mass_flux_water[_qp]*_grad_test[_i][_qp]*_phi[_j][_qp];
}
