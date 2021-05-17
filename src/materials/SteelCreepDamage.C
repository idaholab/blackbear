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

#include "SteelCreepDamage.h"

#include "RankTwoTensor.h"
#include "RankFourTensor.h"
#include "ElasticityTensorTools.h"
#include "MooseUtils.h"
#include <cmath>
#include "RankTwoScalarTools.h"

#include "libmesh/utility.h"

registerMooseObject("BlackBearApp", SteelCreepDamage);

InputParameters
SteelCreepDamage::validParams()
{
  InputParameters params = ScalarDamageBase::validParams();
  params.addClassDescription("Steel scalar damage model: Material 'suddenly' loses load-carrying "
                             "capacity. This can model, e.g., 316H creep failure. See "
                             "'Creep failure simulations of 316H at 550C: Part I - A method and "
                             "validation, by Oh et al, Engineering Fracture Mechanics 78 (2011)'");
  params.addRequiredParam<Real>(
      "epsilon_f",
      "epsilon_f parameter refers to uniaxial creep fracture strain (creep ductility).");
  params.addRequiredParam<Real>("creep_law_exponent", "Exponent of the creep power law.");
  params.addParam<Real>(
      "reduce_factor",
      1000.0,
      "reduce_factor parameter refers to a reduction on the load-carrying capacity (stress).");
  params.addRangeCheckedParam<Real>("initial_damage_value",
                                    0.9,
                                    "initial_damage_value <= 1.0 & initial_damage_value >= 0.0",
                                    "Initial value of damage that will trigger linear reduction of "
                                    "quadrature point's load-carrying capacity.");
  params.addParam<std::string>("creep_strain_name",
                               "creep_strain",
                               "Name of the creep strain material driving damage failure.");
  return params;
}

SteelCreepDamage::SteelCreepDamage(const InputParameters & parameters)
  : ScalarDamageBase(parameters),
    GuaranteeConsumer(this),
    _creep_strain_name(getParam<std::string>("creep_strain_name")),
    _creep_strain(getMaterialProperty<RankTwoTensor>(_base_name + _creep_strain_name)),
    _creep_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + _creep_strain_name)),
    _epsilon_f(getParam<Real>("epsilon_f")),
    _reduce_factor(getParam<Real>("reduce_factor")),
    _initial_damage_value(getParam<Real>("initial_damage_value")),
    _creep_law_exponent(getParam<Real>("creep_law_exponent")),
    _stress(getMaterialProperty<RankTwoTensor>(_base_name + "stress"))
{
  if (_creep_law_exponent == -0.5)
    paramError("creep_law_exponent",
               "creep_law_exponent cannot be -0.5 due to singularities in the multiaxial update of "
               "the uniaxial ductility value.");
}

void
SteelCreepDamage::initQpStatefulProperties()
{
  _damage_index[_qp] = 0.0;
}

const Real &
SteelCreepDamage::getQpDamageIndex(unsigned int /*qp*/)
{
  return _damage_index[_qp];
}

void
SteelCreepDamage::updateDamage()
{
  updateQpDamageIndex();
}

void
SteelCreepDamage::updateQpDamageIndex()
{
  Real epsilon_f_star;
  const Real vonMises = RankTwoScalarTools::vonMisesStress(_stress[_qp]);

  if (vonMises > TOLERANCE)
  {
    const Real h = RankTwoScalarTools::triaxialityStress(_stress[_qp]);

    // Let's only modify axial ductility for significant values of stress triaxiality
    if (h > 0.1)
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
    return;
  }

  RankTwoTensor creep_increment = _creep_strain[_qp] - _creep_strain_old[_qp];

  // Get equivalent creep strain increment
  Real equivalent_creep_increment =
      1 / std::sqrt(2) *
      std::sqrt(Utility::pow<2>(creep_increment(0, 0) - creep_increment(1, 1)) +
                Utility::pow<2>(creep_increment(1, 1) - creep_increment(2, 2)) +
                Utility::pow<2>(creep_increment(2, 2) - creep_increment(0, 0)) +
                1.5 * creep_increment(0, 1) * creep_increment(0, 1) +
                1.5 * creep_increment(1, 2) * creep_increment(1, 2) +
                1.5 * creep_increment(2, 0) * creep_increment(2, 0));

  _damage_index[_qp] = _damage_index_old[_qp] + _dt * equivalent_creep_increment / epsilon_f_star;

  if (_damage_index[_qp] > 1.0)
    _damage_index[_qp] = 1.0;
}

void
SteelCreepDamage::updateStressForDamage(RankTwoTensor & stress_new)
{
  // Reduce stress by a factor when damage reaches a value close to one.
  Real threshold;

  // use_old_damage should be set to true for this object to yield good convergence properties.
  if (_use_old_damage)
    threshold = _damage_index_old[_qp];
  else
    threshold = _damage_index[_qp];

  if (threshold < _initial_damage_value)
    return;

  if (threshold > 1.0)
    threshold = 1;

  stress_new /= (threshold - _initial_damage_value) / (1 - _initial_damage_value) * _reduce_factor;
}

void
SteelCreepDamage::computeUndamagedOldStress(RankTwoTensor & stress_old)
{
  Real damage_index_old = _use_old_damage ? _damage_index_older[_qp] : _damage_index_old[_qp];

  if (damage_index_old < _initial_damage_value)
    return;

  if (damage_index_old > 1.0)
    damage_index_old = 1;

  stress_old *=
      (damage_index_old - _initial_damage_value) / (1 - _initial_damage_value) * _reduce_factor;
}

void
SteelCreepDamage::updateJacobianMultForDamage(RankFourTensor & jacobian_mult)
{
  Real threshold;
  if (_use_old_damage)
    threshold = _damage_index_old[_qp];
  else
    threshold = _damage_index[_qp];

  if (threshold < _initial_damage_value)
    return;

  if (threshold > 1.0)
    threshold = 1;

  jacobian_mult /=
      (threshold - _initial_damage_value) / (1 - _initial_damage_value) * _reduce_factor;
}

Real
SteelCreepDamage::computeTimeStepLimit()
{
  return std::numeric_limits<Real>::max();

  Real current_damage_increment = (_damage_index[_qp] - _damage_index_old[_qp]);
  if (MooseUtils::absoluteFuzzyEqual(current_damage_increment, 0.0))
    return std::numeric_limits<Real>::max();

  return _dt * _maximum_damage_increment / current_damage_increment;
}
