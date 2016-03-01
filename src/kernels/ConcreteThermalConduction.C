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

#include "ConcreteThermalConduction.h"

// If we use a material pointer we need to include the
// material class
#include "Material.h"

template<>
InputParameters validParams<ConcreteThermalConduction>()
{
  InputParameters params = validParams<Diffusion>();
  params.addParam<std::string>("property_name", "thermal_conductivity", "Thermal conductivity");
  return params;
}

ConcreteThermalConduction::ConcreteThermalConduction(const InputParameters & parameters) :
    Diffusion(parameters),
    _thermal_conductivity(getMaterialProperty<Real>("thermal_conductivity"))
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
  return  _thermal_conductivity[_qp] * Diffusion::computeQpJacobian();
}

Real ConcreteThermalConduction::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
