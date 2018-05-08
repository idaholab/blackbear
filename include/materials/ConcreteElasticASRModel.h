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

#ifndef CONCRETEELASTICASRMODEL_H
#define CONCRETEELASTICASRMODEL_H

#include "ConstitutiveModel.h"
#include "SymmIsotropicElasticityTensor.h"

class ConcreteElasticASRModel : public ConstitutiveModel
{
public:
  ConcreteElasticASRModel(const InputParameters & parameters);

protected:
  /// Compute the stress (sigma += deltaSigma)
  virtual void computeStress(const Elem & current_elem,
                             const SymmElasticityTensor & elasticity_tensor,
                             const SymmTensor & stress_old,
                             SymmTensor & strain_increment,
                             SymmTensor & stress_new) override;

  virtual bool modifyStrainIncrement(const Elem & /*elem*/,
                                     SymmTensor & strain_increment,
                                     SymmTensor & d_strain_dT) override;

  virtual bool updateElasticityTensor(SymmElasticityTensor & elasticityTensor) override;

  const bool _ASR_E;
  const Real _E;
  const Real _nu;
  const Real _beta_E;

  const MaterialProperty<Real> & _ASR_extent;
};

template <>
InputParameters validParams<ConcreteElasticASRModel>();

#endif // CONCRETEELASTICASRMODEL_H
