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

#include "ConcreteExpansionMicrocrackingDamage.h"
#include "ElasticityTensorTools.h"

registerMooseObject("BlackBearApp", ConcreteExpansionMicrocrackingDamage);

template <>
InputParameters
validParams<ConcreteExpansionMicrocrackingDamage>()
{
  InputParameters params = validParams<ScalarDamageBase>();
  params.addClassDescription("Scalar damage model based on extent of internal expansion");

  params.addRequiredParam<MaterialPropertyName>(
      "microcracking_eigenstrain_name",
      "Name of the eigenstrain driving the microcracking damage process");

  params.addParam<bool>("assume_isotropic_expansion",
                        true,
                        "Indicates whether the model assumes an isotropic expansion (true) or "
                        "computes the linear expansion based on the first principal eigenstrain "
                        "(false)");

  params.addParam<bool>("include_confinement_effects",
                        true,
                        "Indicates whether the damage is affected by the current stress state");

  params.addParam<Real>("eigenstrain_factor",
                        1.0,
                        "Correction factor by which the eigenstrain is multiplied before "
                        "evaluating the damage");

  params.addRequiredRangeCheckedParam<Real>(
      "microcracking_initiation_strain",
      "microcracking_initiation_strain > 0",
      "Linear strain at which the microcracking initiates (in [m/m])");

  params.addRequiredRangeCheckedParam<Real>(
      "microcracking_strain_branch",
      "microcracking_strain_branch > 0",
      "Parameter controlling the rate at which the microcracking increases (in [m/m])");

  params.addParam<Real>(
      "expansion_stress_limit",
      "Upper bound compressive stress beyond which damage is controlled by the external stress");

  return params;
}

ConcreteExpansionMicrocrackingDamage::ConcreteExpansionMicrocrackingDamage(
    const InputParameters & parameters)
  : ScalarDamageBase(parameters),
    GuaranteeConsumer(this),
    _eigenstrain_name(getParam<MaterialPropertyName>("microcracking_eigenstrain_name")),
    _eigenstrain(getMaterialProperty<RankTwoTensor>(_eigenstrain_name)),
    _eigenstrain_old(getMaterialPropertyOld<RankTwoTensor>(_eigenstrain_name)),
    _assume_isotropic_expansion(getParam<bool>("assume_isotropic_expansion")),
    _eigenstrain_factor(getParam<Real>("eigenstrain_factor")),
    _epsilon_init(getParam<Real>("microcracking_initiation_strain")),
    _epsilon_branch(getParam<Real>("microcracking_strain_branch")),
    _include_confinement_effects(getParam<bool>("include_confinement_effects")),
    _sigma_u(isParamValid("expansion_stress_limit") ? getParam<Real>("expansion_stress_limit")
                                                    : 0.0),
    _stress(getMaterialPropertyOld<RankTwoTensor>("stress")),
    _elasticity_tensor_name(_base_name + "elasticity_tensor"),
    _elasticity_tensor(getMaterialPropertyByName<RankFourTensor>(_elasticity_tensor_name)),
    _eigenvalues(3, 0.0)
{
  if (_include_confinement_effects && !parameters.isParamSetByUser("expansion_stress_limit"))
    paramError("expansion_stress_limit",
               "is a required parameter for include_confinement_effects = true");

  if (_include_confinement_effects && !(_sigma_u > 0))
    paramError("expansion_stress_limit",
               "needs to be strictly > 0 for include_confinement_effects = true");
}

void
ConcreteExpansionMicrocrackingDamage::initialSetup()
{
  if (!hasGuaranteedMaterialProperty(_elasticity_tensor_name, Guarantee::ISOTROPIC))
    mooseError("ConcreteExpansionMicrocrackingDamage "
               "requires that the elasticity tensor be guaranteed isotropic");
}

void
ConcreteExpansionMicrocrackingDamage::updateQpDamageIndex()
{
  const Real linear_expansion = computeLinearExpansion(_eigenstrain[_qp]);
  const Real inc_linear_expansion =
      linear_expansion - computeLinearExpansion(_eigenstrain_old[_qp]);

  _damage_index[_qp] = _damage_index_old[_qp];

  // no additional expansion implies no additional damage
  if (inc_linear_expansion < TOLERANCE)
    return;

  // unconfined damage
  Real inc_damage_unconfined = 0.0;
  if (linear_expansion > _epsilon_init)
  {
    const Real linear_expansion_eq =
        inc_linear_expansion +
        std::max(0.0,
                 _epsilon_init + _epsilon_branch * (1.0 + 1.0 / (1.0 - _damage_index_old[_qp])));
    const Real next_damage_unconfined =
        1.0 - _epsilon_branch / (linear_expansion_eq - (_epsilon_branch + _epsilon_init));
    inc_damage_unconfined = std::max(0.0, next_damage_unconfined - _damage_index_old[_qp]);
  }

  // no stress control implies damage from unconfined expansion only
  if (!_include_confinement_effects)
  {
    _damage_index[_qp] = std::min(1.0, _damage_index_old[_qp] + inc_damage_unconfined);
    return;
  }

  // confined damage
  Real inc_damage_confined = 0.0;

  // sum of compressive stress (positive value)
  _stress[_qp].symmetricEigenvalues(_eigenvalues);
  const Real sigma_compressive = -std::min(0.0, _eigenvalues[0]) - std::min(0.0, _eigenvalues[1]) -
                                 std::min(0.0, _eigenvalues[2]);

  if (sigma_compressive > 0.0)
  {
    const Real E = ElasticityTensorTools::getIsotropicYoungsModulus(_elasticity_tensor[_qp]);
    const Real confinement_factor = E / std::max(_sigma_u, sigma_compressive);
    const Real linear_expansion_eq =
        inc_linear_expansion +
        std::max(0.0,
                 _damage_index_old[_qp] / (confinement_factor * (1.0 - _damage_index_old[_qp])));

    const Real next_damage_confined = 1.0 - 1.0 / (1.0 + confinement_factor * linear_expansion_eq);
    inc_damage_confined = std::max(0.0, next_damage_confined - _damage_index_old[_qp]);
  }

  // combined damage as combination of unconfined + confined
  const Real coef = std::max(0.0, std::min(1.0, sigma_compressive / _sigma_u));
  _damage_index[_qp] = std::min(
      1.0,
      _damage_index_old[_qp] +
          std::max(0.0, coef * inc_damage_confined + (1.0 - coef) * inc_damage_unconfined));
}

Real
ConcreteExpansionMicrocrackingDamage::computeLinearExpansion(const RankTwoTensor & strain)
{
  // the expansion is assumed isotropic
  if (_assume_isotropic_expansion)
    return std::max(0.0, strain(0, 0) * _eigenstrain_factor);

  // otherwise we use the principal expansion directions
  strain.symmetricEigenvalues(_eigenvalues);
  return std::max(std::max(0.0, _eigenvalues[0] * _eigenstrain_factor),
                  std::max(std::max(0.0, _eigenvalues[1] * _eigenstrain_factor),
                           std::max(0.0, _eigenvalues[2] * _eigenstrain_factor)));
}
