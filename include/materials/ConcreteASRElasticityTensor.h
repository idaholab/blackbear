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

#ifndef CONCRETEASRELASTICITYTENSOR_H
#define CONCRETEASRELASTICITYTENSOR_H

#include "ComputeElasticityTensorBase.h"

class ConcreteASRElasticityTensor;

template <>
InputParameters validParams<ConcreteASRElasticityTensor>();

/**
 * ConcreteASRElasticityTensor computes an elasticity tensor that depends
 * on the extent of the alkali-silica rection in concrete.
 */
class ConcreteASRElasticityTensor : public ComputeElasticityTensorBase
{
public:
  ConcreteASRElasticityTensor(const InputParameters & parameters);

protected:
  virtual void initialSetup() override;
  virtual void computeQpElasticityTensor() override;

  /// Young's Modulus
  const Real & _youngs_modulus;

  /// Poisson's Ratio
  const Real & _poissons_ratio;

  /// Residual fraction of youngs_modulus at full ASR reaction
  const Real & _beta_e;

  /// ASR extent (computed externally to this model)
  const MaterialProperty<Real> & _ASR_extent;
};

#endif // CONCRETEASRELASTICITYTENSOR_H
