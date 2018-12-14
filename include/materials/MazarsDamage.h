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

#include "ScalarDamageBase.h"
#include "GuaranteeConsumer.h"

// Forward declaration
class MazarsDamage;
template <typename>
class RankTwoTensorTempl;
typedef RankTwoTensorTempl<Real> RankTwoTensor;
template <typename>
class RankFourTensorTempl;
typedef RankFourTensorTempl<Real> RankFourTensor;

template <>
InputParameters validParams<MazarsDamage>();

/**
 * Scalar damage model that defines the damage parameter using a material property
 */
class MazarsDamage : public ScalarDamageBase, public GuaranteeConsumer
{
public:
  MazarsDamage(const InputParameters & parameters);

  virtual void initQpStatefulProperties() override;
  virtual void initialSetup() override;

protected:
  virtual void updateQpDamageIndex() override;

  /// Tensile strength of material
  const VariableValue & _tensile_strength;

  ///@{ Parameters that control the shape of the nonlinear material response
  const Real & _a_t;
  const Real & _a_c;
  const Real & _b_t;
  const Real & _b_c;
  ///@}

  ///@{ Internal damage variable kappa that tracks the maximum equivalent tensile strain
  MaterialProperty<Real> & _kappa;
  const MaterialProperty<Real> & _kappa_old;
  ///@}

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
