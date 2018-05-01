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

#include "ConcreteASREigenstrain.h"
#include "InputParameters.h"

registerMooseObject("BlackBearApp", ConcreteASREigenstrain);

template <>
InputParameters
validParams<ConcreteASREigenstrain>()
{
  InputParameters params = validParams<ConcreteExpansionEigenstrainBase>();
  params.makeParamRequired<Real>("compressive_strength");
  params.makeParamRequired<Real>("tensile_strength");

  params.addRequiredCoupledVar("temperature", "Coupled temperature");
  params.addRequiredCoupledVar("relative_humidity", "Coupled relative humidity");

  params.addRangeCheckedParam<Real>(
      "rh_exponent",
      4.0,
      "rh_exponent >= 0.0",
      "Power to which relative humidity is raised in computation of ASR volumetric strain");

  params.addRequiredRangeCheckedParam<Real>(
      "max_volumetric_expansion",
      "max_volumetric_expansion > 0.0",
      "Final ansymptotic ASR volumetric expansion strain when reaction is complete");
  params.addRequiredRangeCheckedParam<Real>(
      "characteristic_time",
      "characteristic_time > 0.0",
      "Chracteristic ASR time (in days) at reference temprature. (tau_C(T_0))");
  params.addRequiredRangeCheckedParam<Real>(
      "latency_time",
      "latency_time > 0.0",
      "Latency ASR time (in days) at reference temprature (tau_L(T_0))");
  params.addRangeCheckedParam<Real>("characteristic_activation_energy",
                                    5400.0,
                                    "characteristic_activation_energy > 0.0",
                                    "Activation energy associated with characteristic_time (U_C)");
  params.addRangeCheckedParam<Real>("latency_activation_energy",
                                    9400.0,
                                    "latency_activation_energy > 0.0",
                                    "Activation energy associated with latency_time (U_L)");
  params.addRequiredParam<Real>("reference_temperature",
                                "Reference temperature for ASR reaction constants.");

  // Note that Fahrenheit is not supported because that would require different parameters for the
  // times and activation energies
  MooseEnum temperature_units("Celsius Kelvin");
  params.addRequiredParam<MooseEnum>(
      "temperature_unit",
      temperature_units,
      "Unit used to define 'temperature' and 'reference_temperature'");

  params.addRangeCheckedParam<Real>(
      "stress_latency_factor",
      4.0 / 3.0,
      "stress_latency_factor >= 0.0",
      "Constant for ASR latency time retardation under hydrostatic compression (alpha)");

  params.addRangeCheckedParam<Real>(
      "tensile_absorption_factor",
      0.5,
      "tensile_absorption_factor >= 0.0 & tensile_absorption_factor <= 1.0",
      "Fraction of tensile strength beyond which ASR gel is absorbed into tensile cracks "
      "(gamma_t)");
  params.addRangeCheckedParam<Real>(
      "tensile_retention_factor",
      0.5,
      "tensile_retention_factor >= 0.0 & tensile_retention_factor <= 1.0",
      "Residual ASR retention factor under tension (gamma_r)");

  params.addParam<Real>("compressive_stress_exponent",
                        0.5,
                        "Exponent for ASR retention factor under compressive stress state (beta)");

  params.addParam<bool>("ASR_dependent_tensile_strength",
                        false,
                        "Set true to turn ASR reaction dependent tensile strength");
  params.addRequiredRangeCheckedParam<Real>(
      "residual_tensile_strength_fraction",
      "residual_tensile_strength_fraction >= 0.0 & residual_tensile_strength_fraction <= 1.0",
      "Residual fraction of tensile strength at full ASR reaction");

  params.addParamNamesToGroup("stress_latency_factor tensile_absorption_factor "
                              "tensile_retention_factor compressive_stress_exponent",
                              "Advanced");

  params.addParam<unsigned int>(
      "max_its", 30, "Maximum number of iterations for material solution");
  params.addParam<bool>(
      "output_iteration_info", false, "Set true to output material iteration information");
  params.addParam<bool>("output_iteration_info_on_error",
                        false,
                        "Set true to output material iteration information when a step fails");
  params.addParam<Real>(
      "relative_tolerance", 1e-8, "Relative convergence tolerance for material iteration");
  params.addParam<Real>(
      "absolute_tolerance", 1e-20, "Absolute convergence tolerance for material iteration");

  params.addParamNamesToGroup("max_its output_iteration_info output_iteration_info_on_error "
                              "relative_tolerance absolute_tolerance",
                              "Advanced");

  params.addClassDescription(
      "Computes the volumetric expansion eigenstrain due to alkali-silica reaction.");

  return params;
}

ConcreteASREigenstrain::ConcreteASREigenstrain(const InputParameters & parameters)
  : ConcreteExpansionEigenstrainBase(parameters, "ASR"),
    _temperature(coupledValue("temperature")),

    _relative_humidity(coupledValue("relative_humidity")),
    _rh_exponent(getParam<Real>("rh_exponent")),

    _max_vol_expansion(getParam<Real>("max_volumetric_expansion")),
    _tau_c_T0(getParam<Real>("characteristic_time")),
    _tau_L_T0(getParam<Real>("latency_time")),
    _Uc(getParam<Real>("characteristic_activation_energy")),
    _UL(getParam<Real>("latency_activation_energy")),
    _ref_temp(getParam<Real>("reference_temperature")),
    _alpha(getParam<Real>("stress_latency_factor")),

    _gamma_tensile(getParam<Real>("tensile_absorption_factor")),
    _gamma_residual(getParam<Real>("tensile_retention_factor")),
    _beta(getParam<Real>("compressive_stress_exponent")),

    _ASR_dependent_tensile_strength(getParam<bool>("ASR_dependent_tensile_strength")),
    _beta_f(getParam<Real>("residual_tensile_strength_fraction")),

    _max_its(parameters.get<unsigned int>("max_its")),
    _output_iteration_info(getParam<bool>("output_iteration_info")),
    _output_iteration_info_on_error(getParam<bool>("output_iteration_info_on_error")),
    _relative_tolerance(parameters.get<Real>("relative_tolerance")),
    _absolute_tolerance(parameters.get<Real>("absolute_tolerance")),

    _ASR_extent(declareProperty<Real>("ASR_extent")),
    _ASR_extent_old(getMaterialPropertyOld<Real>("ASR_extent"))
{
  const MooseEnum temperature_unit = getParam<MooseEnum>("temperature_unit");

  if (temperature_unit == "Celsius")
    _temp_offset = 273.15;
  else if (temperature_unit == "Kelvin")
    _temp_offset = 0.0;
  else
    mooseError("Unsupported temperature unit");

  // Convert input value of reference temperature to Kelvin
  _ref_temp += _temp_offset;
}

void
ConcreteASREigenstrain::initQpStatefulProperties()
{
  _ASR_extent[_qp] = 0.0;
  ConcreteExpansionEigenstrainBase::initQpStatefulProperties();
}

Real
ConcreteASREigenstrain::computeQpVolumetricStrain()
{
  // Use Newton iteration to determine ASR reaction extent at new time step
  Real scalar = _ASR_extent_old[_qp];
  unsigned int it = 0;
  Real residual = 10;
  Real norm_residual = 10;
  Real first_norm_residual = 10;

  std::stringstream iter_output;
  while (it < _max_its && norm_residual > _absolute_tolerance &&
         (norm_residual / first_norm_residual) > _relative_tolerance)
  {
    residual = computeResidual(_qp, scalar);
    norm_residual = std::abs(residual);
    if (it == 0)
    {
      first_norm_residual = norm_residual;
      if (first_norm_residual == 0)
        first_norm_residual = 1;
    }

    scalar -= residual / computeDerivative(_qp, scalar);

    if (_output_iteration_info == true || _output_iteration_info_on_error == true)
    {
      iter_output << " it=" << it << " ASR_extent=" << scalar
                  << " rel_res=" << norm_residual / first_norm_residual
                  << " rel_tol=" << _relative_tolerance << " abs_res=" << norm_residual
                  << " abs_tol=" << _absolute_tolerance << std::endl;
    }
    ++it;
  }

  if (_output_iteration_info)
    _console << iter_output.str();
  if (it == _max_its && norm_residual > _absolute_tolerance &&
      (norm_residual / first_norm_residual) > _relative_tolerance)
  {
    if (_output_iteration_info_on_error)
      Moose::err << iter_output.str();
    mooseError("Max material iteration hit during nonlinear constitutive model solve!");
  }

  // new ASR reaction extent
  _ASR_extent[_qp] = scalar;
  Real inc_ASR_extent = _ASR_extent[_qp] - _ASR_extent_old[_qp];

  // stress dependent total ASR volumetric accounting for ASR gel absorption due to tensile and
  // compressive cracking Eigen solve - Note the eigen values are ranked from minimum to maximum
  const Real sig_k = _stress_eigenvalues[2];

  Real gel_absorption_tensile = 1.0;
  Real gel_absorption_compress = 1.0;

  // Optionally calculate effect of ASR reaction on the tensile strength
  Real f_t = _f_tensile;
  if (_ASR_dependent_tensile_strength)
    f_t = _f_tensile * (1.0 - (1.0 - _beta_f) * _ASR_extent[_qp]);

  if (sig_k > _gamma_tensile * f_t)
    gel_absorption_tensile =
        _gamma_residual + (1.0 - _gamma_residual) * (_gamma_tensile * f_t / sig_k);

  Real sig_effective = _stress[_qp].trace() / (3.0 * -_f_compress);

  if (sig_effective > 0.0)
  {
    gel_absorption_compress =
        1.0 - std::exp(_beta) * sig_effective / (1.0 + (std::exp(_beta) - 1.0) * sig_effective);
    if (gel_absorption_compress > 1.0)
      gel_absorption_compress = 1.0;
    else if (gel_absorption_compress < 0.0)
      gel_absorption_compress = 0.0;
  }

  const Real inc_ASR_volumetric_strain = gel_absorption_tensile * gel_absorption_compress *
                                         std::pow(_relative_humidity[_qp], _rh_exponent) *
                                         inc_ASR_extent * _max_vol_expansion;

  return _volumetric_strain_old[_qp] + inc_ASR_volumetric_strain;
}

Real
ConcreteASREigenstrain::computeResidual(unsigned qp, Real scalar)
{
  Real f;
  if (_expansion_type == ExpansionType::Isotropic)
    f = 1.0;
  else if (_expansion_type == ExpansionType::Anisotropic)
  {
    const Real I_sigma = _stress[_qp].trace();
    if (I_sigma >= 0.0) // hydrostatic tension
      f = 1.0;
    else // hydrostatic compression: retarding ASR rection
    {
      f = 1.0 + _alpha * I_sigma / (3.0 * -_f_compress);
      if (f < 1.0)
        mooseError("Wrong retardation for ASR latency time calculation!");
    }
  }
  else
    mooseError("Invalid expansion type");

  // Convert current temperature to Kelvin
  const Real T = _temperature[qp] + _temp_offset;

  // ASR characteristic and latency times (in days)
  Real tau_c = _tau_c_T0 * std::exp(_Uc * (1.0 / T - 1.0 / _ref_temp));
  Real tau_L = f * _tau_L_T0 * std::exp(_UL * (1.0 / T - 1.0 / _ref_temp));

  Real resid = scalar - _ASR_extent_old[qp] -
               (_dt * (1.0 - scalar) * (scalar + std::exp(-tau_L / tau_c))) /
                   (86400.0 * tau_c * (1.0 + std::exp(-tau_L / tau_c)));

  return resid;
}

Real
ConcreteASREigenstrain::computeDerivative(unsigned qp, Real scalar)
{
  Real f;
  if (_expansion_type == ExpansionType::Isotropic)
    f = 1.0;
  else if (_expansion_type == ExpansionType::Anisotropic)
  {
    const Real I_sigma = _stress[_qp].trace();
    if (I_sigma >= 0.0) // hydrostatic tension
      f = 1.0;
    else // hydrostatic compression: retarding ASR rection
    {
      f = 1.0 + _alpha * I_sigma / (3.0 * -_f_compress);
      if (f < 1.0)
        mooseError("Wrong retardation for ASR latency time calculation!");
    }
  }
  else
    mooseError("Invalid expansion type");

  // Convert current temperature to Kelvin
  const Real T = _temperature[qp] + _temp_offset;

  // ASR characteristic and latency times (in days)
  Real tau_c = _tau_c_T0 * std::exp(_Uc * (1.0 / T - 1.0 / _ref_temp));
  Real tau_L = f * _tau_L_T0 * std::exp(_UL * (1.0 / T - 1.0 / _ref_temp));

  Real jac = 1.0 - _dt / 86400.0 / tau_c * (1.0 - scalar) / (1.0 + std::exp(-tau_L / tau_c)) -
             _dt / 86400.0 / tau_c * (-1) / (1.0 + std::exp(-tau_L / tau_c)) *
                 (scalar + std::exp(-tau_L / tau_c));
  return jac;
}
