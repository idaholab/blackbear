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

#include "ComputeStressBase.h"

#include "neml_interface.h"

/**
 * NEMLStressBase uses the NEML constitutive model library from ANL to
 * compute the stress, and track dissipation and strain energy.
 */

class NEMLStressBase : public ComputeStressBase
{
public:
  static InputParameters validParams();
  NEMLStressBase(const InputParameters & parameters);

  virtual void computeQpStress() override;
  virtual void initQpStatefulProperties() override;

  /**
   * Translates a RankTwoTensor object to a NEML tensor stored in a vector
   * format.
   * @param in  RankTwoTensor to be translated
   * @param out NEML vector output
   */
  static void rankTwoTensorToNeml(const RankTwoTensor & in, double * const out);

  /**
   * Translates a NEML tensor stored in vector format to a RankTwoTensor.
   * @param in  NEML vector to be translated
   * @param out RankTwoTensor output
   */
  static void nemlToRankTwoTensor(const double * const in, RankTwoTensor & out);

  /**
   * Translates a NEML elasticity tensor to a RankFourTensor.
   * @param in  NEML elasticity tensor to be translated
   * @param out RankFourTensor output
   */
  static void nemlToRankFourTensor(const double * const in, RankFourTensor & out);

protected:
  /// NEML model
  std::unique_ptr<neml::NEMLModel> _model;

  ///@{ History variables used by NEML model
  MaterialProperty<std::vector<Real>> & _hist;
  const MaterialProperty<std::vector<Real>> & _hist_old;
  ///@}

  /// Old mechanical strain
  const MaterialProperty<RankTwoTensor> & _mechanical_strain_old;

  /// Old stress
  const MaterialProperty<RankTwoTensor> & _stress_old;

  ///@{ Strain energy
  MaterialProperty<Real> & _energy;
  const MaterialProperty<Real> & _energy_old;
  ///@}

  ///@{ Dissipation
  MaterialProperty<Real> & _dissipation;
  const MaterialProperty<Real> & _dissipation_old;
  ///@}

  ///@{ Coupled temperature variable (defaults to zero if not specified)
  const VariableValue & _temperature;
  const VariableValue & _temperature_old;
  ///@}

  /// Inelastic strain tensor
  MaterialProperty<RankTwoTensor> & _inelastic_strain;

  ///@{ Computation of a material based time step limit
  const bool _compute_dt;
  const Real _target_increment;
  const MaterialProperty<RankTwoTensor> * _inelastic_strain_old;
  MaterialProperty<Real> * _material_dt;
  ///@}

  ///Material property for the damage index only used when there is a damage model
  MaterialProperty<Real> * _damage_index;
  /// Print debugging data on failed NEML stress updates
  const bool _debug;
};
