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

#include "ConcreteMoistureDiffusion.h"

// If we use a material pointer we need to include the
// material class
#include "Material.h"

template<>
InputParameters validParams<ConcreteMoistureDiffusion>()
{
  InputParameters params = validParams<Diffusion>();
  params.addCoupledVar("coupled_vars", "coupld to temperature");
  return params;
}

ConcreteMoistureDiffusion::ConcreteMoistureDiffusion(const InputParameters & parameters) :
    Diffusion(parameters),
    _Dh(getMaterialProperty<Real>("humidity_diffusivity")),
    _Dht(getMaterialProperty<Real>("humidity_diffusivity_thermal"))
{
  int n = coupledComponents("coupled_vars");
  _dvals_dxyz.resize(n);

  for (unsigned int i = 0; i < _dvals_dxyz.size(); ++i)
    _dvals_dxyz[i] = &coupledGradient("coupled_vars", i);
}

Real
ConcreteMoistureDiffusion::computeQpResidual()
{
  // We're dereferencing the _diffusivity pointer to get to the
  // material properties vector... which gives us one property
  // value per quadrature point.

  // Also... we're reusing the Diffusion Kernel's residual
  // so that we don't have to recode that.
  //  if (_u[_qp]>=0.0)

  Real re =  _Dh[_qp] * Diffusion::computeQpResidual();

  if (_dvals_dxyz.size() == 1)
    re += _Dht[_qp] * _grad_test[_i][_qp] * (*_dvals_dxyz[0])[_qp];

  return re;
}

Real
ConcreteMoistureDiffusion::computeQpJacobian()
{
  // We're dereferencing the _diffusivity pointer to get to the
  // material properties vector... which gives us one property
  // value per quadrature point.

  // Also... we're reusing the Diffusion Kernel's residual
  // so that we don't have to recode that.
  return  _Dh[_qp] * Diffusion::computeQpJacobian();
}

Real ConcreteMoistureDiffusion::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
