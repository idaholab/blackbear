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

#ifndef CONCRETELOGARITHMICCREEPMODEL_H
#define CONCRETELOGARITHMICCREEPMODEL_H

#include "GeneralizedKelvinVoigtBase.h"

class ConcreteLogarithmicCreepModel;

template<>
InputParameters validParams<ConcreteLogarithmicCreepModel>();

/**
 * Creep model for concrete adapted from [Benboudjema and Torrenti, 2012]
 *
 * Long-term creep follows a logarithmic curve, while short-term creep is
 * controlled by a single Kelvin-Voigt module (which may be diasbled).
 *
 * The model can account for temperature (accelerates creep using an
 * Arrhenius law) and humidity (reduces creep under constant humidity,
 * increases creep under drying conditions).
 */
class ConcreteLogarithmicCreepModel : public GeneralizedKelvinVoigtBase
{
public:
  ConcreteLogarithmicCreepModel(const InputParameters & parameters);

protected:
  virtual void computeQpViscoelasticProperties() override;

  /// Initial elasticity tensor
  RankFourTensor _C0;

  ///@{ Controls the recoverable deformation
  bool _has_recoverable_deformation;
  RankFourTensor _Cr;
  Real _recoverable_dashpot_viscosity;
  ///@}

  ///@{ Controls the long_term irrecoverable deformation
  Real _long_term_dashpot_viscosity;
  Real _long_term_dashpot_characteristic_time;
  ///@}

  ///@{ Temperature coupling
  bool _has_temp;
  Real _temperature_reference;
  Real _creep_activation_temperature;
  const VariableValue & _temperature;
  ///@}

  ///@{ Humidity coupling
  bool _has_humidity;
  const VariableValue & _humidity;
  const VariableValue & _humidity_old;
  ///@}

  ///@{ Drying coupling
  bool _has_drying_creep;
  Real _drying_creep_viscosity;
  ///@}
};

#endif // CONCRETELOGARITHMICCREEPMODEL_H
