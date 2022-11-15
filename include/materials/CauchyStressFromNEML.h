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

#include "ComputeLagrangianStressCauchy.h"

#include "neml_interface.h"

class CauchyStressFromNEML : public ComputeLagrangianStressCauchy
{
public:
  static InputParameters validParams();
  CauchyStressFromNEML(const InputParameters & parameters);

protected:
  virtual void computeQpCauchyStress();
  virtual void initQpStatefulProperties();

protected:
  FileName _fname;
  std::string _mname;
  std::unique_ptr<neml::NEMLModel> _model;

  const VariableValue & _temperature;
  const VariableValue & _temperature_old;

  MaterialProperty<std::vector<Real>> & _history;
  const MaterialProperty<std::vector<Real>> & _history_old;

  MaterialProperty<Real> & _energy;
  const MaterialProperty<Real> & _energy_old;

  MaterialProperty<Real> & _dissipation;
  const MaterialProperty<Real> & _dissipation_old;

  MaterialProperty<RankTwoTensor> & _linear_rotation;
  const MaterialProperty<RankTwoTensor> & _linear_rotation_old;

  const MaterialProperty<RankTwoTensor> & _cauchy_stress_old;

  const MaterialProperty<RankTwoTensor> & _mechanical_strain;
  const MaterialProperty<RankTwoTensor> & _mechanical_strain_old;

  MaterialProperty<RankTwoTensor> & _inelastic_strain;
  MaterialProperty<RankTwoTensor> & _elastic_strain;

  MaterialProperty<Real> & _dissipation_rate;
};

/// Tensor -> skew vector
void rankTwoSkewToNEML(const RankTwoTensor & in, double * const out);

/// Skew + symmetric parts to full tangent
void
recombineTangentNEML(const double * const Dpart, const double * const Wpart, RankFourTensor & out);
