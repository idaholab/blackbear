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

#include "SteelCreepDamageOh.h"

#include "RankTwoTensor.h"
#include "RankFourTensor.h"
#include "ElasticityTensorTools.h"
#include "MooseUtils.h"
#include <cmath>
#include "RankTwoScalarTools.h"

#include "libmesh/utility.h"

registerMooseObject("BlackBearApp", SteelCreepDamageOh);
registerMooseObject("BlackBearApp", ADSteelCreepDamageOh);

template <bool is_ad>
InputParameters
SteelCreepDamageOhTempl<is_ad>::validParams()
{
  InputParameters params = ScalarDamageBaseTempl<is_ad>::validParams();
  params.addClassDescription(
      "Steel scalar damage model: Material 'suddenly' loses load-carrying "
      "capacity. This can model, e.g., 316H creep failure. See Oh et al (2011).");
  params.addRequiredParam<Real>("epsilon_f", "Uniaxial creep fracture strain (creep ductility).");
  params.addRequiredParam<Real>("creep_law_exponent", "Exponent of the creep power law.");
  params.addParam<Real>(
      "reduction_factor", 1000.0, "Reduction on the load-carrying capacity (stress).");
  params.addRangeCheckedParam<Real>(
      "reduction_damage_threshold",
      0.9,
      "reduction_damage_threshold <= 1.0 & reduction_damage_threshold >= 0.0",
      "Starting value of damage that will trigger linear reduction of "
      "quadrature point's load-carrying capacity.");
  params.addParam<std::string>("creep_strain_name",
                               "creep_strain",
                               "Name of the creep strain material driving damage failure.");
  return params;
}

template <bool is_ad>
SteelCreepDamageOhTempl<is_ad>::SteelCreepDamageOhTempl(const InputParameters & parameters)
  : ScalarDamageBaseTempl<is_ad>(parameters),
    GuaranteeConsumer(this),
    _creep_strain_name(this->template getParam<std::string>("creep_strain_name")),
    _creep_strain(this->template getGenericMaterialProperty<RankTwoTensor, is_ad>(
        _base_name + _creep_strain_name)),
    _creep_strain_old(
        this->template getMaterialPropertyOld<RankTwoTensor>(_base_name + _creep_strain_name)),
    _epsilon_f(this->template getParam<Real>("epsilon_f")),
    _reduction_factor(this->template getParam<Real>("reduction_factor")),
    _reduction_damage_threshold(this->template getParam<Real>("reduction_damage_threshold")),
    _creep_law_exponent(this->template getParam<Real>("creep_law_exponent")),
    _stress(this->template getGenericMaterialProperty<RankTwoTensor, is_ad>(_base_name + "stress")),
    _omega(this->template declareGenericProperty<Real, is_ad>(_base_name + "omega")),
    _omega_old(this->template getMaterialPropertyOld<Real>(_base_name + "omega"))
{
  if (MooseUtils::absoluteFuzzyEqual(_creep_law_exponent, -0.5, TOLERANCE))
    this->template paramError(
        "creep_law_exponent",
        "creep_law_exponent cannot be -0.5 due to singularities in the multiaxial update of "
        "the uniaxial ductility value.");
}

template <bool is_ad>
void
SteelCreepDamageOhTempl<is_ad>::initQpStatefulProperties()
{
  ScalarDamageBaseTempl<is_ad>::initQpStatefulProperties();
  _omega[_qp] = 0.0;
}

template <bool is_ad>
void
SteelCreepDamageOhTempl<is_ad>::updateQpDamageIndex()
{
  Real epsilon_f_star;

  const auto & stress = MetaPhysicL::raw_value(_stress[_qp]);
  const auto & vonMises = RankTwoScalarTools::vonMisesStress(stress);

  if (vonMises > TOLERANCE)
  {
    const auto & h = RankTwoScalarTools::triaxialityStress(stress);

    // Let's only modify axial ductility for significant values of stress triaxiality
    if (h > TOLERANCE)
      // Update creep ductility due to triaxiliaty effects
      epsilon_f_star =
          _epsilon_f *
          std::sinh(2.0 / 3.0 * (_creep_law_exponent - 0.5) / (_creep_law_exponent + 0.5)) /
          std::sinh(2.0 * (_creep_law_exponent - 0.5) / (_creep_law_exponent + 0.5) * h);
    else
      epsilon_f_star = _epsilon_f;
  }
  else
    epsilon_f_star = _epsilon_f;

  // If the value obtained from multiaxiality equation isn't reasonable, do not update damage
  // This would also cause an FPE a few lines below
  if (std::abs(epsilon_f_star) < TOLERANCE)
  {
    _damage_index[_qp] = _damage_index_old[_qp];
    _omega[_qp] = _omega_old[_qp];
    return;
  }

  GenericRankTwoTensor<is_ad> creep_increment = _creep_strain[_qp] - _creep_strain_old[_qp];

  // Avoid derivative's divide by zero error
  Real epsilon_ad = 1.0e-14;

  // Get equivalent creep strain increment
  GenericReal<is_ad> equivalent_creep_increment =
      1 / std::sqrt(2) *
      std::sqrt(Utility::pow<2>(creep_increment(0, 0) - creep_increment(1, 1)) +
                Utility::pow<2>(creep_increment(1, 1) - creep_increment(2, 2)) +
                Utility::pow<2>(creep_increment(2, 2) - creep_increment(0, 0)) +
                1.5 * creep_increment(0, 1) * creep_increment(0, 1) +
                1.5 * creep_increment(1, 2) * creep_increment(1, 2) +
                1.5 * creep_increment(2, 0) * creep_increment(2, 0) + epsilon_ad);

  _omega[_qp] = _omega_old[_qp] + equivalent_creep_increment / epsilon_f_star;

  if (_omega[_qp] > 1.0)
    _omega[_qp] = 1.0;

  Real threshold = MetaPhysicL::raw_value(_omega[_qp]);

  if (threshold < _reduction_damage_threshold)
  {
    // If threshold is not reached, there is no damage.
    _damage_index[_qp] = 0.0;
    return;
  }

  if (threshold > 1.0)
    threshold = 1;

  // Cast damage index from the omega model to the traditional damage index.
  Real factor = (threshold - _reduction_damage_threshold) / (1 - _reduction_damage_threshold) *
                _reduction_factor;
  _damage_index[_qp] = 1.0 - 1.0 / factor;

  // Account for a corner case where threshold == _reduction_damage_threshold
  if (_damage_index[_qp] < 0.0)
    _damage_index[_qp] = 0.0;
}

template class SteelCreepDamageOhTempl<false>;
template class SteelCreepDamageOhTempl<true>;
