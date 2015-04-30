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

class ConcreteElasticASRModel : public ConstitutiveModel
{
public:
  ConcreteElasticASRModel( const std::string & name,
                InputParameters parameters );
  virtual ~ConcreteElasticASRModel();

  virtual void initStatefulProperties(unsigned int n_points);

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
  virtual bool applyASRStrain(unsigned qp, SymmTensor & strain_increment);
  virtual Real computeResidual(unsigned qp,  Real scalar);
  virtual Real computeDerivative(unsigned qp, Real scalar);


  bool _has_rh; //coupled to relative humidity
  VariableValue & _rh;

// parameters associated with ASR volumtric explasion
  Real _ASR_final_vstrain;
  Real _tau_c_T0;
  Real _tau_L_T0;
  Real _Uc;
  Real _UL;
  Real _ASR_T0;

  const unsigned int _max_its;
  const bool _output_iteration_info;
  const bool _output_iteration_info_on_error;
  const Real _relative_tolerance;
  const Real _absolute_tolerance;

  MaterialProperty<Real> & _ASR_extent;
  MaterialProperty<Real> & _ASR_extent_old;

  MaterialProperty<Real> & _ASR_volumetric_strain;
  MaterialProperty<Real> & _ASR_volumetric_strain_old;



};

template<>
InputParameters validParams<ConcreteElasticASRModel>();


#endif
