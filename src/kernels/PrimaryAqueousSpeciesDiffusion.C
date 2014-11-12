#include "PrimaryAqueousSpeciesDiffusion.h"

// If we use a material pointer we need to include the
// material class
#include "Material.h"

template<>
InputParameters validParams<PrimaryAqueousSpeciesDiffusion>()
{
  InputParameters params = validParams<Diffusion>();
  params.addParam<std::string>("property_name","diffusivity","DEFAULT: diffusivity");
  return params;
}

PrimaryAqueousSpeciesDiffusion::PrimaryAqueousSpeciesDiffusion(const std::string & name, InputParameters parameters)
  :Diffusion(name,parameters),
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
    return _porosity[_qp] * _diffusivity[_qp]*Diffusion::computeQpResidual();

}

Real
PrimaryAqueousSpeciesDiffusion::computeQpJacobian()
{
  // We're dereferencing the _diffusivity pointer to get to the
  // material properties vector... which gives us one property
  // value per quadrature point.

  // Also... we're reusing the Diffusion Kernel's residual
  // so that we don't have to recode that.
  return _porosity[_qp] * _diffusivity[_qp]*Diffusion::computeQpJacobian();
}

Real PrimaryAqueousSpeciesDiffusion::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
