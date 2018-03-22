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

template<>
InputParameters validParams<ConcreteThermalConvection>()
{
  InputParameters params = validParams<Kernel>();
  return params;
}

ConcreteThermalConvection::ConcreteThermalConvection(const InputParameters & parameters) :
    Kernel(parameters),
    _cw(getMaterialProperty<Real>("thermal_capacity_of_water")),
    _darcy_moisture_flux(getMaterialProperty<RealGradient>("darcy_moisture_flux"))
{
}

Real ConcreteThermalConvection::computeQpResidual()
{
  return _cw[_qp] * _darcy_moisture_flux[_qp] * _test[_i][_qp]*_grad_u[_qp];
  // return -_specific_heat_water[_qp]*_darcy_mass_flux_water[_qp]*_grad_test[_i][_qp]*_u[_qp];
}

Real ConcreteThermalConvection::computeQpJacobian()
{
  return _cw[_qp] * _darcy_moisture_flux[_qp] * _test[_i][_qp]*_grad_phi[_j][_qp];
  // return -_specific_heat_water[_qp]*_darcy_mass_flux_water[_qp]*_grad_test[_i][_qp]*_phi[_j][_qp];
}
