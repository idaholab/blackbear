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

#ifndef MAZARSDAMAGE_H
#define MAZARSDAMAGE_H

#include "DamageBase.h"
#include "GuaranteeConsumer.h"

// Forward declaration
class MazarsDamage;
class RankTwoTensor;
class RankFourTensor;

template <>
InputParameters validParams<MazarsDamage>();

/**
 * Scalar damage model that defines the damage parameter using a material property
 */
class MazarsDamage : public DamageBase, public GuaranteeConsumer
{
public:
  MazarsDamage(const InputParameters & parameters);

  virtual void initQpStatefulProperties() override;
  virtual void initialSetup() override;
  virtual void updateDamage() override;
  virtual void updateStressForDamage(RankTwoTensor & stress_new) override;
  virtual void updateJacobianMultForDamage(RankFourTensor & jacobian_mult) override;

protected:
  /// If true, use the damage index from the old state (rather than the current state)
  const bool _use_old_damage;

  /// Tensile strength of material
  const Real & _tensile_strength;

  ///@{ Parameters that control the shape of the nonlinear material response
  const Real & _a_t;
  const Real & _a_c;
  const Real & _b_t;
  const Real & _b_c;
  ///@}

  /// Residual fraction of stiffness used for material that is fully damaged
  const Real & _residual_stiffness_fraction;

  /// Internal damage variable kappa that tracks the maximum equivalent tensile strain
  MaterialProperty<Real> & _kappa;

  /// Old value of kappa
  const MaterialProperty<Real> & _kappa_old;

  /// Damage index
  MaterialProperty<Real> & _damage_index;

  /// Old state of the damage index
  const MaterialProperty<Real> & _damage_index_old;

  /// Current stress
  const MaterialProperty<RankTwoTensor> & _stress;

  /// Current mechanical strain
  const MaterialProperty<RankTwoTensor> & _mechanical_strain;

  /// Name of elasticity tensor
  const std::string _elasticity_tensor_name;

  /// Current undamaged elasticity tensor
  const MaterialProperty<RankFourTensor> & _elasticity_tensor;

  /// Computed eigenvalues. Re-used for efficiency.
  std::vector<Real> _eigval;

  /// Positive components of stress tensor. Re-used for efficiency.
  std::vector<Real> _positive_stress;

  /// Positive components of strain tensor. Re-used for efficiency.
  std::vector<Real> _positive_strain;
};

#endif // MAZARSDAMAGE_H
