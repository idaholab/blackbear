#include "Material.h"
#include "ConcreteThermalConvection.h"

template<>
InputParameters validParams<ConcreteThermalConvection>()
{
  InputParameters params = validParams<Kernel>();
  return params;
}

ConcreteThermalConvection::ConcreteThermalConvection(const std::string & name, InputParameters parameters)
  :Kernel(name, parameters),
   _cw(getMaterialProperty<Real>("thermal_capacity_of_water")),
   _darcy_moisture_flux(getMaterialProperty<RealGradient>("darcy_moisture_flux"))
{}

Real ConcreteThermalConvection::computeQpResidual()
{
  return _cw[_qp]*_darcy_moisture_flux[_qp] * _test[_i][_qp]*_grad_u[_qp];

//  return -_specific_heat_water[_qp]*_darcy_mass_flux_water[_qp]*_grad_test[_i][_qp]*_u[_qp];
}

Real ConcreteThermalConvection::computeQpJacobian()
{
  return _cw[_qp]*_darcy_moisture_flux[_qp]*_test[_i][_qp]*_grad_phi[_j][_qp];

//    return -_specific_heat_water[_qp]*_darcy_mass_flux_water[_qp]*_grad_test[_i][_qp]*_phi[_j][_qp];
}

