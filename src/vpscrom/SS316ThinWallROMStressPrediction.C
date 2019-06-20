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

#include "SS316ThinWallROMStressPrediction.h"

registerMooseObject("BlackBearApp", SS316ThinWallROMStressPrediction);

template <>
InputParameters
validParams<SS316ThinWallROMStressPrediction>()
{
  InputParameters params = validParams<ComputeFiniteStrainElasticStress>();
  params.addClassDescription("Calculates the stress based on the inelastic strains predicted by a material specific Reduced Order Model derived from a Visco-Plastic Self Consistent model for SS316.");

  params.addRequiredCoupledVar("temperature", "The coupled temperature (K)");
  params.addParam<Real>("initial_mobile_dislocation_density", 0.0, "Initial density of mobile (glissile) dislocations (1/m^2)");
  params.addParam<Real>("initial_immobile_dislocation_density", 0.0, "Immobile (locked) dislocation density initial value (1/m^2)");

  return params;
}

SS316ThinWallROMStressPrediction::SS316ThinWallROMStressPrediction(const InputParameters & parameters)
  : ComputeFiniteStrainElasticStress(parameters),
    _temperature(coupledValue("temperature")),
    _mobile_dislocations(declareProperty<Real>(_base_name + "mobile_dislocations")),
    _mobile_dislocations_old(getMaterialPropertyOld<Real>(_base_name + "mobile_dislocations")),
    _initial_mobile_dislocations(getParam<Real>("initial_mobile_dislocation_density")),
    _immobile_dislocations(declareProperty<Real>(_base_name + "immobile_dislocations")),
    _immobile_dislocations_old(getMaterialPropertyOld<Real>(_base_name + "immobile_dislocations")),
    _initial_immobile_dislocations(getParam<Real>("initial_immobile_dislocation_density")),
    _elastic_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "elastic_strain")),
    _strain_increment(getMaterialProperty<RankTwoTensor>(_base_name + "strain_increment")),
    _inelastic_strain(declareProperty<RankTwoTensor>(_base_name + "rom_inelastic_strain")),
    _inelastic_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "rom_inelastic_strain"))
{
}

void
SS316ThinWallROMStressPrediction::initQpStatefulProperties()
{
  if (!hasGuaranteedMaterialProperty(_elasticity_tensor_name, Guarantee::ISOTROPIC))
    mooseError("SS316ThinWallROMStressPrediction can only be used with elasticity tensor materials "
               "that guarantee isotropic tensors.");
  ComputeStressBase::initQpStatefulProperties();
  _inelastic_strain[_qp].zero();
  _mobile_dislocations[_qp] = _initial_mobile_dislocations;
  _immobile_dislocations[_qp] = _initial_immobile_dislocations;
}

void
SS316ThinWallROMStressPrediction::computeQpStress()
{
// shamelessly copying useful code (or potentially useful) over from other classes
  RankTwoTensor elastic_strain_increment;
  RankTwoTensor inelastic_strain_increment;

  elastic_strain_increment = _strain_increment[_qp];

  // Use the old elastic strain here because we require tensors used by this class
  // to be isotropic and this method natively allows for changing in time
  // elasticity tensors
  _stress[_qp] = _elasticity_tensor[_qp] * (_elastic_strain_old[_qp] + elastic_strain_increment);


/// Then this should be the actual ROM model call
  InterfaceWithROM(inelastic_strain_increment);

  // After have built the strain tensor from the components returned by the ROM class
  // Update the strain tensors and the stress tensor, following the approach in
  // ComputeMultipleInelasticStress

  elastic_strain_increment -= inelastic_strain_increment;

  _elastic_strain[_qp] = _elastic_strain_old[_qp] + elastic_strain_increment;
  _inelastic_strain[_qp] = _inelastic_strain_old[_qp] + inelastic_strain_increment;

  _stress[_qp] = _elasticity_tensor[_qp] * (elastic_strain_increment + _elastic_strain_old[_qp]);

  // update the rotations, assuming finite strain and finite rotation, copy from
  // ComputeMultipleInelasticStress::finiteStrainRotation
  _elastic_strain[_qp] =
      _rotation_increment[_qp] * _elastic_strain[_qp] * _rotation_increment[_qp].transpose();
  _inelastic_strain[_qp] =
      _rotation_increment[_qp] * _inelastic_strain[_qp] * _rotation_increment[_qp].transpose();
  _stress[_qp] = _rotation_increment[_qp] * _stress[_qp] * _rotation_increment[_qp].transpose();

  // Compute dstress_dstrain approximation
  _Jacobian_mult[_qp] = _elasticity_tensor[_qp]; // This is NOT the exact jacobian
}

void
SS316ThinWallROMStressPrediction::InterfaceWithROM(RankTwoTensor & inelastic_strain_increment)
{
  // Declare the increment variables
  Real delta_mobile_disl = 0.0;
  Real delta_immobile_disl = 0.0;
  Real delta_effective_strain = 0.0;
  Real delta_hoop_strain = 0.0;
  Real delta_radial_strain = 0.0;
  Real delta_axial_strain = 0.0;

  // Calculate the ROM inputs
  const Real vmstress = std::sqrt(2.0 / 3.0 * _stress[_qp].doubleContraction(_stress[_qp])) * 1.0e-6; // Convert to MPa
  const Real effective_strain_old = std::sqrt(2.0 / 3.0 * _elastic_strain_old[_qp].doubleContraction(_elastic_strain_old[_qp]));

  std::cout << "Need to see what is going into the rom model: \n";
  std::cout << "  dt: " << _dt << "\n";
  std::cout << "  old mobile disl: " << _mobile_dislocations_old[_qp] << "\n";
  std::cout << "  old immobile disl: " << _immobile_dislocations_old[_qp] << "\n";
  std::cout << "  trial stress into rom: " << vmstress << "\n";
  std::cout << "  old effective strain: " << effective_strain_old << "\n";
  std::cout << "  temperature: " << _temperature[_qp] << "\n";

  _ss316_rom_calculations.computeROMPredictions(_dt, _mobile_dislocations_old[_qp], _immobile_dislocations_old[_qp], vmstress, effective_strain_old, _temperature[_qp], delta_mobile_disl, delta_immobile_disl, delta_effective_strain);

    std::cout << "ROM outputs \n";
    std::cout << "  rom_effective_strain: " << delta_effective_strain <<"\n";
    std::cout << "  mobile disl increment: " << delta_mobile_disl << "\n";
    std::cout << "  immobile disl increment: " << delta_immobile_disl << "\n";

  // Assign calculated increment values, assuming all calculated strain is inelastic
  inelastic_strain_increment(0,0) = delta_radial_strain;
  inelastic_strain_increment(1,1) = delta_axial_strain;
  inelastic_strain_increment(2,2) = delta_hoop_strain;

  _mobile_dislocations[_qp] = _mobile_dislocations_old[_qp] + delta_mobile_disl;
  _immobile_dislocations[_qp] = _immobile_dislocations_old[_qp] + delta_immobile_disl;
}
