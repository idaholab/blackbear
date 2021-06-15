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

  const GenericMaterialProperty<RankTwoTensor, is_ad> & _creep_strain;
  const MaterialProperty<RankTwoTensor> & _creep_strain_old;

  const Real _epsilon_f;

  const Real _reduce_factor;

  const Real _initial_damage_value;

  const Real _creep_law_exponent;

  const GenericMaterialProperty<RankTwoTensor, is_ad> & _stress;

  using ScalarDamageBaseTempl<is_ad>::_damage_index;
  using ScalarDamageBaseTempl<is_ad>::_damage_index_name;
  using ScalarDamageBaseTempl<is_ad>::_damage_index_old;
  using ScalarDamageBaseTempl<is_ad>::_damage_index_older;
  using ScalarDamageBaseTempl<is_ad>::_qp;
  using ScalarDamageBaseTempl<is_ad>::_use_old_damage;
  using ScalarDamageBaseTempl<is_ad>::_dt;
  using ScalarDamageBaseTempl<is_ad>::_base_name;
  using ScalarDamageBaseTempl<is_ad>::_maximum_damage_increment;
};

typedef SteelCreepDamageTempl<false> SteelCreepDamage;
typedef SteelCreepDamageTempl<true> ADSteelCreepDamage;
