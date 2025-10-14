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

#include "ConcreteLogarithmicCreepModel.h"

registerMooseObject("BlackBearApp", ConcreteLogarithmicCreepModel);

InputParameters
ConcreteLogarithmicCreepModel::validParams()
{
  InputParameters params = GeneralizedKelvinVoigtBase::validParams();
  params.addRequiredParam<Real>("youngs_modulus", "Initial elastic modulus of the material");
  params.setDocUnit("youngs_modulus", "stress");
  params.addRequiredParam<Real>("poissons_ratio", "Initial poisson ratio of the material");
  params.addParam<Real>("recoverable_youngs_modulus",
                        "Modulus corresponding to the recoverable part of the deformation");
  params.setDocUnit("recoverable_youngs_modulus", "stress");
  params.addParam<Real>("recoverable_poissons_ratio",
                        "Poisson coefficient of the recoverable part of the deformation");
  params.addRangeCheckedParam<Real>(
      "recoverable_viscosity",
      "recoverable_viscosity > 0",
      "Viscosity corresponding to the recoverable part of the deformation");
  params.setDocUnit("recoverable_viscosity", "time");
  params.addRequiredRangeCheckedParam<Real>(
      "long_term_viscosity",
      "long_term_viscosity > 0",
      "Viscosity corresponding to the long-term part of the deformation");
  params.setDocUnit("long_term_viscosity", "time");
  params.addRangeCheckedParam<Real>("long_term_characteristic_time",
                                    1,
                                    "long_term_characteristic_time > 0",
                                    "Rate at which the long_term viscosity increases");
  params.setDocUnit("long_term_characteristic_time", "time");
  params.addCoupledVar("temperature", "Temperature variable");
  params.setDocUnit("temperature", "degC");
  params.addRangeCheckedParam<Real>("activation_temperature",
                                    "activation_temperature >= 0",
                                    "Activation temperature for the creep");
  params.setDocUnit("activation_temperature", "K");
  params.addParam<Real>("reference_temperature", 20, "Reference temperature");
  params.setDocUnit("reference_temperature", "degC");
  params.addCoupledVar("humidity", "Humidity variable");
  params.addRangeCheckedParam<Real>("drying_creep_viscosity",
                                    "drying_creep_viscosity > 0",
                                    "Viscosity corresponding to the drying creep");
  params.setDocUnit("drying_creep_viscosity", "time");
  params.addParam<bool>("use_recovery", true, "Enables or disables creep recovery");
  params.addClassDescription("Logarithmic viscoelastic model for cementitious materials.");
  return params;
}

ConcreteLogarithmicCreepModel::ConcreteLogarithmicCreepModel(const InputParameters & parameters)
  : GeneralizedKelvinVoigtBase(parameters),
    _has_recoverable_deformation(getParam<bool>("use_recovery")),
    _recoverable_dashpot_viscosity(isParamValid("recoverable_viscosity")
                                       ? getParam<Real>("recoverable_viscosity")
                                       : getParam<Real>("long_term_characteristic_time")),
    _long_term_dashpot_viscosity(getParam<Real>("long_term_viscosity")),
    _long_term_dashpot_characteristic_time(getParam<Real>("long_term_characteristic_time")),
    _has_temp(isCoupled("temperature")),
    _temperature_reference(_has_temp ? getParam<Real>("reference_temperature") : 20),
    _creep_activation_temperature(_has_temp ? getParam<Real>("activation_temperature") : 0),
    _temperature(_has_temp ? coupledValue("temperature") : _zero),
    _temperature_old(_has_temp ? coupledValueOld("temperature") : _zero),
    _has_humidity(isCoupled("humidity")),
    _humidity(_has_humidity ? coupledValue("humidity") : _zero),
    _humidity_old(_has_humidity ? coupledValueOld("humidity") : _zero),
    _has_drying_creep(_has_humidity && isParamValid("drying_creep_viscosity")),
    _drying_creep_viscosity(_has_drying_creep ? getParam<Real>("drying_creep_viscosity") : 0)
{
  Real youngs_modulus = getParam<Real>("youngs_modulus");
  Real poissons_ratio = getParam<Real>("poissons_ratio");
  _C0.fillFromInputVector({youngs_modulus, poissons_ratio},
                          RankFourTensor::symmetric_isotropic_E_nu);

  if (_has_recoverable_deformation)
  {
    Real recoverable_youngs_modulus = isParamValid("recoverable_youngs_modulus")
                                          ? getParam<Real>("recoverable_youngs_modulus")
                                          : youngs_modulus;
    Real recoverable_poissons_ratio = isParamValid("recoverable_poissons_ratio")
                                          ? getParam<Real>("recoverable_poissons_ratio")
                                          : poissons_ratio;
    _Cr.fillFromInputVector({recoverable_youngs_modulus, recoverable_poissons_ratio},
                            RankFourTensor::symmetric_isotropic_E_nu);
  }

  if (_has_temp && !isParamValid("activation_temperature"))
    mooseError("missing parameter for ConcreteLogarithmicCreepModel: activation_temperature");

  if (!_has_humidity && isParamValid("drying_creep_viscosity"))
    mooseError("missing coupled variable for ConcreteLogarithmicCreepModel: humidity");

  _components = (_has_recoverable_deformation ? 2 : 1);
  _has_longterm_dashpot = true;

  issueGuarantee(_elasticity_tensor_name, Guarantee::ISOTROPIC);
  declareViscoelasticProperties();
}

void
ConcreteLogarithmicCreepModel::computeQpViscoelasticProperties()
{
  _first_elasticity_tensor[_qp] = _C0;

  // temperature correction if need be
  Real temperature_coeff = 1.;
  if (_has_temp)
    temperature_coeff =
        std::exp(_creep_activation_temperature *
                 (1. / (273.15 + (_temperature[_qp] + _temperature_old[_qp]) * 0.5) -
                  1. / (273.15 + _temperature_reference)));

  // recoverable deformation if need be
  if (_has_recoverable_deformation)
  {
    (*_springs_elasticity_tensors[0])[_qp] = _Cr;
    (*_dashpot_viscosities[0])[_qp] = _recoverable_dashpot_viscosity * temperature_coeff;
  }

  // basic long-term creep
  (*_dashpot_viscosities.back())[_qp] =
      _long_term_dashpot_viscosity *
      (1. + std::max(_t - _dt * 0.5, 0.) / _long_term_dashpot_characteristic_time);

  if (_has_humidity)
  {
    Real humidity_coef = 1. / std::max((_humidity[_qp] + _humidity_old[_qp]) * 0.5, 1e-6);
    (*_dashpot_viscosities.back())[_qp] *= humidity_coef;
    if (_has_recoverable_deformation)
      (*_springs_elasticity_tensors[0])[_qp] *= humidity_coef;

    // basic and drying long-term creep are assembled in series
    // (equivalent viscosity is the geometric mean of the viscosities)
    if (_has_drying_creep)
    {
      Real dh_dt = (_humidity[_qp] - _humidity_old[_qp]) / _dt;
      if (!MooseUtils::absoluteFuzzyEqual(dh_dt, 0.0))
      {
        Real dc_visc = _drying_creep_viscosity / std::abs(dh_dt);
        Real bc_visc = (*_dashpot_viscosities.back())[_qp];

        (*_dashpot_viscosities.back())[_qp] = 1. / (1. / bc_visc + 1. / dc_visc);
      }
    }
  }

  (*_dashpot_viscosities.back())[_qp] *= temperature_coeff;
}
