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

#include "ConcreteElasticASRModel.h"

template<>
InputParameters validParams<ConcreteElasticASRModel>()
{
  InputParameters params = validParams<ConstitutiveModel>();
  params.addParam<bool>("ASR_dependent_E", false, "whether youngs modulus depends on ASR reaction or not?");
  params.addParam<Real>("youngs_modulus", 26.0e9, "initial youngs_modulus of concrete");
  params.addParam<Real>("poissons_ratio", 0.2 , "Poissons_ratios of concrete");
  params.addParam<Real>("beta_E", 0.5, "residual fraction of youngs_modulus at full ASR reaction [0,1]");
  return params;
}

ConcreteElasticASRModel::ConcreteElasticASRModel(const InputParameters & parameters) :
    ConstitutiveModel(parameters),
    _ASR_E(getParam<bool>("ASR_dependent_E")),
    _E(getParam<Real>("youngs_modulus")),
    _nu(getParam<Real>("poissons_ratio")),
    _beta_E(getParam<Real>("beta_E")),
    _ASR_extent(getMaterialProperty<Real>("ASR_extent"))
{
}

void
ConcreteElasticASRModel::computeStress(const Elem & /*current_elem*/,
                                       unsigned /*qp*/,
                                       const SymmElasticityTensor & elasticity_tensor,
                                       const SymmTensor & stress_old,
                                       SymmTensor & strain_increment,
                                       SymmTensor & stress_new )
{
  stress_new = elasticity_tensor * strain_increment;
  stress_new += stress_old;
}

bool
ConcreteElasticASRModel::modifyStrainIncrement(const Elem & /*elem*/,
                                               unsigned /*qp*/,
                                               SymmTensor & /*strain_increment*/,
                                               SymmTensor & /*d_strain_dT*/)
{
  bool modified = false;
  return modified;
}

bool
ConcreteElasticASRModel::updateElasticityTensor(unsigned qp, SymmElasticityTensor & elasticityTensor)
{
  bool modified = false;

  if (_ASR_E)
  {
    SymmIsotropicElasticityTensor * t = dynamic_cast<SymmIsotropicElasticityTensor*>(&elasticityTensor);
    if (!t)
      mooseError("Cannot use Youngs modulus or Poissons ratio functions");

    t->constant(false);
    t->unsetConstants();

    const Real E = _E *(1.0 - (1.0 - _beta_E) * _ASR_extent[qp]);
    t->setYoungsModulus(E);
    t->setPoissonsRatio(_nu);

    modified = true;
  }

  return modified;
}
