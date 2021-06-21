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

#pragma once

#include "ScalarDamageBase.h"
#include "GuaranteeConsumer.h"

/**
 * Scalar damage model that defines the damage parameter using a material property
 */
template <bool is_ad>
class SteelCreepDamageTempl : public ScalarDamageBaseTempl<is_ad>, public GuaranteeConsumer
{
public:
  static InputParameters validParams();

  SteelCreepDamageTempl(const InputParameters & parameters);

  virtual void initQpStatefulProperties() override;

  virtual void updateDamage() override;

  virtual void updateStressForDamage(GenericRankTwoTensor<is_ad> & stress_new) override;

  virtual void updateJacobianMultForDamage(RankFourTensor & jacobian_mult) override;

  virtual void computeUndamagedOldStress(RankTwoTensor & stress_old) override;

  virtual Real computeTimeStepLimit() override;

  const GenericReal<is_ad> & getQpDamageIndex(unsigned int qp);

  const std::string getDamageIndexName() const { return _damage_index_name; }

protected:
  virtual void updateQpDamageIndex() override;

  std::string _creep_strain_name;

  ///@{ Creep strain tensors
  const GenericMaterialProperty<RankTwoTensor, is_ad> & _creep_strain;
  const MaterialProperty<RankTwoTensor> & _creep_strain_old;
  ///@}

  /// Uniaxial creep fracture strain (used to compute multiaxial creep fracture strain)
  /// This parameter usually take values that range from 0.05 to 0.2
  /// "Creep failure simulations of 316H at 550C: Part I - A method and
  /// validation", Oh et al, Engineering Fracture Mechanics 78 (2011)
  const Real _epsilon_f;

  /// Positive real to reduce the amount of stress at the quadrature point.
  /// The larger the the value, the less the load-carrying capacity of the element will be.
  const Real _reduce_factor;

  /// Initial damage value at which we will start reducing linearly the element's load
  ///carrying capacity. initial_damage_value < 1
  const Real _initial_damage_value;

  /// Value to match the power law creep model used as a material
  /// If material is defined by multiple creep models, this value represents
  /// an average of creep behavior.
  const Real _creep_law_exponent;

  /// Generic stress tensor
  const GenericMaterialProperty<RankTwoTensor, is_ad> & _stress;

  ///@{ Make hierarchy parameters available in this class
  using ScalarDamageBaseTempl<is_ad>::_damage_index;
  using ScalarDamageBaseTempl<is_ad>::_damage_index_name;
  using ScalarDamageBaseTempl<is_ad>::_damage_index_old;
  using ScalarDamageBaseTempl<is_ad>::_damage_index_older;
  using ScalarDamageBaseTempl<is_ad>::_qp;
  using ScalarDamageBaseTempl<is_ad>::_use_old_damage;
  using ScalarDamageBaseTempl<is_ad>::_dt;
  using ScalarDamageBaseTempl<is_ad>::_base_name;
  using ScalarDamageBaseTempl<is_ad>::_maximum_damage_increment;
  ///@}
};

typedef SteelCreepDamageTempl<false> SteelCreepDamage;
typedef SteelCreepDamageTempl<true> ADSteelCreepDamage;
