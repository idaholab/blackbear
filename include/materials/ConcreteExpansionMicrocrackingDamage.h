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

#ifndef CONCRETEEXPANSIONMICROCRACKINGDAMAGE_H
#define CONCRETEEXPANSIONMICROCRACKINGDAMAGE_H

#include "ScalarDamageBase.h"
#include "GuaranteeConsumer.h"

// Forward declaration
class ConcreteExpansionMicrocrackingDamage;

template <>
InputParameters validParams<ConcreteExpansionMicrocrackingDamage>();

/**
 * Scalar damage model that is driven by the evolution of an internal eigenstrain that represents
 * the expansion of aggregates
 */
class ConcreteExpansionMicrocrackingDamage : public ScalarDamageBase, public GuaranteeConsumer
{
public:
  ConcreteExpansionMicrocrackingDamage(const InputParameters & parameters);

  virtual void initialSetup() override;

protected:
  virtual void updateQpDamageIndex() override;

  Real computeLinearExpansion(const RankTwoTensor & strain);

  /// Name of the eigenstrain that drives the microcracking
  const std::string _eigenstrain_name;

  ///@{ Eigenstrain that stores the expansion
  const MaterialProperty<RankTwoTensor> & _eigenstrain;
  const MaterialProperty<RankTwoTensor> & _eigenstrain_old;
  ///@}

  /// Determines whether we need to get all expansion components
  const bool _use_isotropic_expansion;

  /// Correction factor applied to the eigenstrain before the damage calculation
  /// (If negative, the damage will be driven by the shrinkage of concrete rather than the expansion)
  const Real _correction_factor;

  /// Linear strain at which microcracking initiates in unconfined conditions (in [m/m])
  const Real _epsilon_init;

  /// Coefficient controlling the microcracking rate in unconfined conditions (in [m/m])
  const Real _epsilon_branch;

  /// Option to activates the effect of compressive stress on microcracking
  const bool _use_stress_control;

  /// Upper bound compressive stress beyond which no expansion occurs (in Pascals)
  const Real _sigma_u;

  /// Reference to stress tensor (computed outside this model)
  const MaterialProperty<RankTwoTensor> & _stress;
  
  /// Name of elasticity tensor
  const std::string _elasticity_tensor_name;

  /// Current undamaged elasticity tensor
  const MaterialProperty<RankFourTensor> & _elasticity_tensor;

  /// Eigenvalues of the strain or stress tensor for current qp (computed only if needed)
  std::vector<Real> _eigenvalues;
};

#endif // CONCRETEEXPANSIONMICROCRACKINGDAMAGE_H
