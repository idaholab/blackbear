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

#include "RadialReturnCreepStressUpdateBase.h"
#include "SS316ROMStressPrediction.h"

class SS316ROMStressPrediction;

template <>
InputParameters validParams<SS316ROMStressPrediction>();

/**
 * Class to call the Reduced Order Model to predict the behavior of HT9
 * proof of concept state
 */

class SS316ROMStressPrediction : public RadialReturnCreepStressUpdateBase
{
public:
  SS316ROMStressPrediction(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual Real computeResidual(const Real effective_trial_stress, const Real scalar) override;
  virtual Real computeDerivative(const Real effective_trial_stress, const Real scalar) override;
  virtual void computeStressFinalize(const RankTwoTensor & plastic_strain_increment) override;


  /// Provides access to the call method to the standalone ROM code
  SS316ROMPrediction _ss316_rom_calculations;

private:
  const VariableValue & _temperature;

  const MaterialProperty<RankTwoTensor> & _mechanical_strain;
  const MaterialProperty<RankTwoTensor> & _mechanical_strain_old;

  ///@{Mobile (glissile) dislocation densities (1/m^2)
  MaterialProperty<Real> & _mobile_dislocations;
  const MaterialProperty<Real> & _mobile_dislocations_old;
  MaterialProperty<Real> & _mobile_dislocation_increment;
  const Real _initial_mobile_dislocations;
  ///@}

  ///@{Immobile (locked) dislocation densities (1/m^2)
  MaterialProperty<Real> & _immobile_dislocations;
  const MaterialProperty<Real> & _immobile_dislocations_old;
  MaterialProperty<Real> & _immobile_dislocation_increment;
  const Real _initial_immobile_dislocations;
  ///@}
};
