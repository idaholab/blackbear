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

#include "ConcreteASRElasticityTensor.h"

registerMooseObject("BlackBearApp", ConcreteASRElasticityTensor);

template <>
InputParameters
validParams<ConcreteASRElasticityTensor>()
{
  InputParameters params = validParams<ComputeElasticityTensorBase>();
  params.addClassDescription("Compute an elasticity tensor that depends on the extent of the "
                             "alkali-silica rection in concrete");
  params.addRequiredParam<Real>("youngs_modulus", "Young's Modulus");
  params.addRequiredParam<Real>("poissons_ratio", "Poisson's Ratio");
  params.addRequiredRangeCheckedParam<Real>(
      "beta",
      "beta >= 0 & beta <= 1",
      "Residual fraction of youngs_modulus at full ASR reaction [0,1]");
  return params;
}

ConcreteASRElasticityTensor::ConcreteASRElasticityTensor(const InputParameters & parameters)
  : ComputeElasticityTensorBase(parameters),
    _youngs_modulus(getParam<Real>("youngs_modulus")),
    _poissons_ratio(getParam<Real>("poissons_ratio")),
    _beta(getParam<Real>("beta")),
    _ASR_extent(getMaterialProperty<Real>("ASR_extent"))
{
  // The tensor created by this class is always isotropic
  issueGuarantee(_elasticity_tensor_name, Guarantee::ISOTROPIC);
}

void
ConcreteASRElasticityTensor::initialSetup()
{
}

void
ConcreteASRElasticityTensor::computeQpElasticityTensor()
{
  const Real E = _youngs_modulus * (1.0 - (1.0 - _beta) * _ASR_extent[_qp]);
  const Real nu = _poissons_ratio;

  _elasticity_tensor[_qp].fillSymmetricIsotropicEandNu(E, nu);
}
