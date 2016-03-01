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

#include "PrimaryAqueousSpeciesDiffusion.h"

template<>
InputParameters validParams<PrimaryAqueousSpeciesDiffusion>()
{
  InputParameters params = validParams<Diffusion>();
  params.addParam<MaterialPropertyName>("property_name", "diffusivity", "Aqueous species diffusivity");
  return params;
}

PrimaryAqueousSpeciesDiffusion::PrimaryAqueousSpeciesDiffusion(const InputParameters & parameters) :
    Diffusion(parameters),
    _porosity(getMaterialProperty<Real>("porosity")),
    _diffusivity(getMaterialProperty<Real>("diffusivity"))
{
}

Real
PrimaryAqueousSpeciesDiffusion::computeQpResidual()
{
  // We're dereferencing the _diffusivity pointer to get to the
  // material properties vector... which gives us one property
  // value per quadrature point.

  // Also... we're reusing the Diffusion Kernel's residual
  // so that we don't have to recode that.
  //  if (_u[_qp]>=0.0)
  return _porosity[_qp] * _diffusivity[_qp] * Diffusion::computeQpResidual();
}

Real
PrimaryAqueousSpeciesDiffusion::computeQpJacobian()
{
  // We're dereferencing the _diffusivity pointer to get to the
  // material properties vector... which gives us one property
  // value per quadrature point.

  // Also... we're reusing the Diffusion Kernel's residual
  // so that we don't have to recode that.
  return _porosity[_qp] * _diffusivity[_qp] * Diffusion::computeQpJacobian();
}

Real PrimaryAqueousSpeciesDiffusion::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
