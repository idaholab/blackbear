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

#ifndef CONCRETEASRMICROCRACKINGDAMAGE_H
#define CONCRETEASRMICROCRACKINGDAMAGE_H

#include "ScalarDamageBase.h"

// Forward declaration
class ConcreteASRMicrocrackingDamage;

template <>
InputParameters validParams<ConcreteASRMicrocrackingDamage>();

/**
 * Scalar damage model that defines the damage parameter using a material property
 */
class ConcreteASRMicrocrackingDamage : public ScalarDamageBase
{
public:
  ConcreteASRMicrocrackingDamage(const InputParameters & parameters);

protected:
  virtual void updateQpDamageIndex() override;

  /// Residual fraction of youngs_modulus at full ASR reaction
  const Real & _beta_e;

  ///@{ ASR extent (computed externally to this model)
  const MaterialProperty<Real> & _ASR_extent;
};

#endif // CONCRETEASRMICROCRACKINGDAMAGE_H
