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

#include "SS316ROMStressPrediction.h"

registerMooseObject("BlackBearApp", SS316ROMStressPrediction);

template <>
InputParameters
validParams<SS316ROMStressPrediction>()
{
  InputParameters params = validParams<RadialReturnCreepStressUpdateBase>();
  params.addClassDescription("Calculates the effective J2 stress based on the inelastic strains predicted by a material specific Reduced Order Model derived from a Visco-Plastic Self Consistent model for SS316.");

  params.addRequiredCoupledVar("temperature", "The coupled temperature (K)");
  params.addParam<Real>("initial_mobile_dislocation_density", 0.0, "Initial density of mobile (glissile) dislocations (1/m^2)");
  params.addParam<Real>("initial_immobile_dislocation_density", 0.0, "Immobile (locked) dislocation density initial value (1/m^2)");

  return params;
}

SS316ROMStressPrediction::SS316ROMStressPrediction(const InputParameters & parameters)
  : RadialReturnCreepStressUpdateBase(parameters),
    _temperature(coupledValue("temperature")),
    _mobile_dislocations(declareProperty<Real>(_base_name + "mobile_dislocations")),
    _mobile_dislocations_old(getMaterialPropertyOld<Real>(_base_name + "mobile_dislocations")),
    _mobile_dislocation_increment(declareProperty<Real>(_base_name + "mobile_dislocation_increment")),
    _initial_mobile_dislocations(getParam<Real>("initial_mobile_dislocation_density")),
    _immobile_dislocations(declareProperty<Real>(_base_name + "immobile_dislocations")),
    _immobile_dislocations_old(getMaterialPropertyOld<Real>(_base_name + "immobile_dislocations")),
    _immobile_dislocation_increment(declareProperty<Real>(_base_name + "immobile_dislocation_increment")),
    _initial_immobile_dislocations(getParam<Real>("initial_immobile_dislocation_density"))
{
  _line_search = false;
}

void
SS316ROMStressPrediction::initQpStatefulProperties()
{
  _mobile_dislocations[_qp] = _initial_mobile_dislocations;
  _immobile_dislocations[_qp] = _initial_immobile_dislocations;

  RadialReturnCreepStressUpdateBase::initQpStatefulProperties();
}

Real
SS316ROMStressPrediction::computeResidual(const Real effective_trial_stress, const Real scalar)
{
  const Real trial_stress_mpa = (effective_trial_stress - _three_shear_modulus * scalar) * 1.0e-6;
  const Real effective_strain_old = std::sqrt(2.0 / 3.0 * _creep_strain_old[_qp].doubleContraction(_creep_strain_old[_qp]));
  Real rom_effective_strain = 0.0;

  std::cout.precision(9);
  std::cout << std::scientific;
  std::cout << "Need to see what is going into the rom model: \n";
  std::cout << "  dt: " << _dt << "\n";
  std::cout << "  old mobile disl: " << _mobile_dislocations_old[_qp] << "\n";
  std::cout << "  old immobile disl: " << _immobile_dislocations_old[_qp] << "\n";
  std::cout << "  input vonmises devitoric stress (MPa): " << effective_trial_stress * 1.0e-6 << "\n";
  std::cout << "  the scalar strain value: " << scalar << "\n";
  // std::cout << "  value 3G: " << _three_shear_modulus * scalar << "\n";
  std::cout << "  trial stress into rom (MPa): " << trial_stress_mpa << "\n";
  std::cout << "  old effective strain: " << effective_strain_old << "\n";
  std::cout << "  temperature: " << _temperature[_qp] << "\n";

  _ss316_rom_calculations.computeROMPredictions(_dt, _mobile_dislocations_old[_qp], _immobile_dislocations_old[_qp], trial_stress_mpa, effective_strain_old, _temperature[_qp], _mobile_dislocation_increment[_qp], _immobile_dislocation_increment[_qp], rom_effective_strain);

  std::cout << "ROM outputs \n";
  std::cout << "  increment effective strain from rom: " << rom_effective_strain <<"\n";
  std::cout << "  new rom_effective_strain: " << effective_strain_old + rom_effective_strain <<"\n";
  std::cout << "  new mobile dislocations: " << _mobile_dislocations_old[_qp] + _mobile_dislocation_increment[_qp] << "\n";
  std::cout << "  new immobile dislocations: " << _immobile_dislocations_old[_qp] + _immobile_dislocation_increment[_qp] << "\n";
  std::cout << "  different rom_effective_strain - scalar: " << rom_effective_strain - scalar << "\n \n";

  return rom_effective_strain - scalar;
}

Real
SS316ROMStressPrediction::computeDerivative(const Real effective_trial_stress, const Real scalar)
{
  const Real trial_stress_mpa = (effective_trial_stress - _three_shear_modulus * scalar) * 1.0e-6;
  const Real effective_strain_old = std::sqrt(2.0 / 3.0 * _creep_strain_old[_qp].doubleContraction(_creep_strain_old[_qp]));
  Real derivative_rom_effective_strain = 0.0;

  _ss316_rom_calculations.computeROMDerivative(_dt, _mobile_dislocations_old[_qp], _immobile_dislocations_old[_qp], trial_stress_mpa, effective_strain_old, _temperature[_qp], _mobile_dislocation_increment[_qp], _immobile_dislocation_increment[_qp], derivative_rom_effective_strain);

  return derivative_rom_effective_strain - 1.0;
  // return 1.0;
}

void
SS316ROMStressPrediction::computeStressFinalize(
    const RankTwoTensor & plastic_strain_increment)
{
  _creep_strain[_qp] = _creep_strain_old[_qp] + plastic_strain_increment;
  _mobile_dislocations[_qp] = _mobile_dislocations_old[_qp] + _mobile_dislocation_increment[_qp];
  _immobile_dislocations[_qp] = _immobile_dislocations_old[_qp] + _immobile_dislocation_increment[_qp];

  std::cout << "Values calculated in finalize step for qp number:" << _qp << " \n";
  std::cout << "  increment effective creep strain: " << std::sqrt(2.0 / 3.0 * plastic_strain_increment.doubleContraction(plastic_strain_increment)) <<"\n";
  std::cout << "  effective_creep_strain: " << std::sqrt(2.0 / 3.0 * _creep_strain[_qp].doubleContraction(_creep_strain[_qp])) <<"\n";
  std::cout << "  new mobile dislocations: " << _mobile_dislocations[_qp] << "\n";
  std::cout << "  new immobile dislocations: " << _immobile_dislocations[_qp] << "\n \n -------------------------------------- \n";
}
