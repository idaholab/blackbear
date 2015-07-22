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

#ifndef CONCRETEELASTICASRMODEL_H
#define CONCRETEELASTICASRMODEL_H

#include "ConstitutiveModel.h"
#include "SymmIsotropicElasticityTensor.h"

class ConcreteElasticASRModel : public ConstitutiveModel
{
public:
  ConcreteElasticASRModel(const InputParameters & parameters);
  virtual ~ConcreteElasticASRModel();

protected:

  /// Compute the stress (sigma += deltaSigma)
  virtual void computeStress( const Elem & current_elem,
                              unsigned qp,
                              const SymmElasticityTensor & elasticity_tensor,
                              const SymmTensor & stress_old,
                              SymmTensor & strain_increment,
                              SymmTensor & stress_new );
  virtual bool modifyStrainIncrement(const Elem & /*elem*/,
                                     unsigned qp,
                                     SymmTensor & strain_increment,
                                     SymmTensor & d_strain_dT);

  virtual bool updateElasticityTensor(unsigned qp, SymmElasticityTensor & elasticityTensor);




  bool _ASR_E;
  Real _E;
  Real _nu;
  Real _beta_E;


const MaterialProperty<Real> & _ASR_extent;

};

template<>
InputParameters validParams<ConcreteElasticASRModel>();


#endif
