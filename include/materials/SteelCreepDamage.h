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
class SteelCreepDamage : public ScalarDamageBase, public GuaranteeConsumer
{
public:
  static InputParameters validParams();
  SteelCreepDamage(const InputParameters & parameters);

  virtual void initQpStatefulProperties() override;

  virtual void updateDamage() override;

  virtual void updateStressForDamage(RankTwoTensor & stress_new) override;

  virtual void updateJacobianMultForDamage(RankFourTensor & jacobian_mult) override;

  virtual void computeUndamagedOldStress(RankTwoTensor & stress_old) override;

  virtual Real computeTimeStepLimit() override;

  const Real & getQpDamageIndex(unsigned int qp);

  const std::string getDamageIndexName() const { return _damage_index_name; }

protected:
  virtual void updateQpDamageIndex() override;

  std::string _creep_strain_name;

  const MaterialProperty<RankTwoTensor> & _creep_strain;
  const MaterialProperty<RankTwoTensor> & _creep_strain_old;

  const Real _epsilon_f;

  const Real _reduce_factor;

  const Real _initial_damage_value;

  const Real _creep_law_exponent;

  const MaterialProperty<RankTwoTensor> & _stress;
};
