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

#include "ComputeFiniteStrainElasticStress.h"
#include "StressUpdateBase.h"
#include "SS316ROMPrediction.h"

class SS316ThinWallROMStressPrediction;

template <>
InputParameters validParams<SS316ThinWallROMStressPrediction>();

/**
 * Class to call the Reduced Order Model to predict the behavior of SS316 as implict
 * stress calcultion only
 * proof of concept state
 */

class SS316ThinWallROMStressPrediction : public ComputeFiniteStrainElasticStress
{
public:
  SS316ThinWallROMStressPrediction(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpStress() override;

  /**
   * Method to interface with the stand alone ROM code. Converts the relevant
   * material properties and variables into the form expected by the ROM code,
   * calls the ROM code, and reorganizes the results from the ROM class into the
   * form required by the MOOSE apps.
   */
  void InterfaceWithROM(RankTwoTensor & inelastic_strain_increment);

  /// Provides access to the call method for the HT9 ROM code
  SS316ROMPrediction _ss316_rom_calculations;

private:
  const VariableValue & _temperature;

  ///@{Mobile (glissile) dislocation densities (1/m^2)
  MaterialProperty<Real> & _mobile_dislocations;
  const MaterialProperty<Real> & _mobile_dislocations_old;
  const Real _initial_mobile_dislocations;
  ///@}

  ///@{Immobile (locked) dislocation densities (1/m^2)
  MaterialProperty<Real> & _immobile_dislocations;
  const MaterialProperty<Real> & _immobile_dislocations_old;
  const Real _initial_immobile_dislocations;
  ///@}

  ///@{ Strain tensors
  const MaterialProperty<RankTwoTensor> & _elastic_strain_old;
  const MaterialProperty<RankTwoTensor> & _strain_increment;
  ///@}

  /// The sum of the inelastic strains that come from the ROM class
  MaterialProperty<RankTwoTensor> & _inelastic_strain;

  /// old value of inelastic strain
  const MaterialProperty<RankTwoTensor> & _inelastic_strain_old;
};
