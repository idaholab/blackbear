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

#include "ConcreteThermalConduction.h"
#include "Material.h"

registerMooseObject("BlackBearApp", ConcreteThermalConduction);

InputParameters
ConcreteThermalConduction::validParams()
{
  InputParameters params = Diffusion::validParams();
  params.addParam<std::string>("property_name", "thermal_conductivity", "Thermal conductivity");
  params.addClassDescription("Conduction term for thermal transport in concrete.");
  return params;
}

ConcreteThermalConduction::ConcreteThermalConduction(const InputParameters & parameters)
  : Diffusion(parameters), _thermal_conductivity(getMaterialProperty<Real>("thermal_conductivity"))
{
}

Real
ConcreteThermalConduction::computeQpResidual()
{
  // We're dereferencing the _diffusivity pointer to get to the
  // material properties vector... which gives us one property
  // value per quadrature point.

  // Also... we're reusing the Diffusion Kernel's residual
  // so that we don't have to recode that.
  //  if (_u[_qp]>=0.0)
  return _thermal_conductivity[_qp] * Diffusion::computeQpResidual();
}

Real
ConcreteThermalConduction::computeQpJacobian()
{
  // We're dereferencing the _diffusivity pointer to get to the
  // material properties vector... which gives us one property
  // value per quadrature point.

  // Also... we're reusing the Diffusion Kernel's residual
  // so that we don't have to recode that.
  return _thermal_conductivity[_qp] * Diffusion::computeQpJacobian();
}

Real
ConcreteThermalConduction::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
