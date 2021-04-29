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

#include "ConcreteMoistureDiffusion.h"
#include "Material.h"

registerMooseObject("BlackBearApp", ConcreteMoistureDiffusion);

InputParameters
ConcreteMoistureDiffusion::validParams()
{
  InputParameters params = Diffusion::validParams();
  params.addCoupledVar("temperature", 0.0, "Temperature");
  params.addClassDescription(
      "Fickian and Soret diffusion terms for moisture transfer in concrete.");
  return params;
}

ConcreteMoistureDiffusion::ConcreteMoistureDiffusion(const InputParameters & parameters)
  : Diffusion(parameters),
    _Dh(getMaterialProperty<Real>("humidity_diffusivity")),
    _Dht(getMaterialProperty<Real>("humidity_diffusivity_thermal")),
    _grad_T(coupledGradient("temperature"))
{
}

Real
ConcreteMoistureDiffusion::computeQpResidual()
{
  return _Dh[_qp] * Diffusion::computeQpResidual() + _Dht[_qp] * _grad_test[_i][_qp] * _grad_T[_qp];
}

Real
ConcreteMoistureDiffusion::computeQpJacobian()
{
  return _Dh[_qp] * Diffusion::computeQpJacobian();
}

Real
ConcreteMoistureDiffusion::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
