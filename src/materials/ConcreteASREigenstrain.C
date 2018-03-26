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

registerMooseObject("BlackBearApp", ConcreteASREigenstrain);

template <>
InputParameters
validParams<ConcreteASREigenstrain>()
{
  InputParameters params = validParams<ConcreteExpansionEigenstrainBase>();

  MooseEnum expansion_type("Isotropic Anisotropic");
  params.addParam<MooseEnum>(
      "expansion", expansion_type, "Type of expansion resulting from volumetric strain");
  params.addRequiredCoupledVar("temperature", "Coupled temperature");
  params.addRequiredCoupledVar("relative_humidity", "Coupled relative humidity");

  params.addParam<Real>(
      "rh_exponent",
      0.0,
      "Power to which relative humidity is raised in computation of ASR volumetric strain");

  params.addParam<Real>(
      "ASR_vol_expansion",
      0.005,
      "Final ansymptotic ASR volumertic expansion strain when reaction is complete");
  params.addParam<Real>(
      "tau_c_T0", 50.0, "Chracteristic ASR time (in days) at reference temprature");
  params.addParam<Real>("tau_L_T0", 200.0, "Latency ASR time (in days) at reference temprature");
  params.addParam<Real>("Uc", 5400.0, "Activation energy associated with tau_c");
  params.addParam<Real>("UL", 9400.0, "Activation energy associated with tau_L");
  params.addParam<Real>("ASR_T0", 38.0, "Reference temperature associated ASR in Celsius");

  params.addParam<Real>("f_compress", -31.0e6, "Compressive strength of concrete (in Pascals)");
  params.addParam<Real>(
      "alpha", 1.3333, "Constant for ASR latency time retardation under hydrostatic compression");
  params.addParam<Real>("f_tensile", 3.0e6, "Tensile strength of concrete (in Pascals)");

  params.addParam<Real>(
      "gamma_tensile",
      0.5,
      "Fraction of tensile strength beyond which ASR gel is adsorbed into tensile cracks");
  params.addParam<Real>("gamma_residual", 0.5, "Residual ASR retention factor under tension");

  params.addParam<Real>(
      "beta", 0.5, "Exponent for ASR retention factor under compressive stress state");

  params.addParam<bool>("ASR_dependent_tensile_strength",
                        false,
                        "Set true to turn ASR reaction dependent tensile strength");
  params.addParam<Real>(
      "beta_f_tensile", 0.5, "Fractional residual tensile strength at full ASR reaction");

  params.addParam<unsigned int>("max_its", 30, "Maximum number of sub-newton iterations");
  params.addParam<bool>(
      "output_iteration_info", false, "Set true to output sub-newton iteration information");
  params.addParam<bool>("output_iteration_info_on_error",
                        false,
                        "Set true to output sub-newton iteration information when a step fails");
  params.addParam<Real>(
      "relative_tolerance", 1e-5, "Relative convergence tolerance for sub-newtion iteration");
  params.addParam<Real>(
      "absolute_tolerance", 1e-20, "Absolute convergence tolerance for sub-newtion iteration");

  params.addClassDescription(
      "Computes the scalar volumetric expansion due to alkali-silica reaction. "
      "Used in conjunction with another model to apply this as an eigenstrain.");

  return params;
}

ConcreteASREigenstrain::ConcreteASREigenstrain(const InputParameters & parameters)
  : ConcreteExpansionEigenstrainBase(parameters, "ASR"),
    _temperature(coupledValue("temperature")),

    _rh(coupledValue("relative_humidity")),
    _rh_exponent(getParam<Real>("rh_exponent")),

    _expansion(getParam<MooseEnum>("expansion").getEnum<ExpansionType>()),

    _ASR_final_vstrain(getParam<Real>("ASR_vol_expansion")),
    _tau_c_T0(getParam<Real>("tau_c_T0")),
    _tau_L_T0(getParam<Real>("tau_L_T0")),
    _Uc(getParam<Real>("Uc")),
    _UL(getParam<Real>("UL")),
    _ASR_T0(getParam<Real>("ASR_T0")),
    _alpha(getParam<Real>("alpha")),

    _gamma_tensile(getParam<Real>("gamma_tensile")),
    _gamma_residual(getParam<Real>("gamma_residual")),
    _beta(getParam<Real>("beta")),

    _ASR_dependent_tensile_strength(getParam<bool>("ASR_dependent_tensile_strength")),
    _beta_f_tensile(getParam<Real>("beta_f_tensile")),

    _max_its(parameters.get<unsigned int>("max_its")),
    _output_iteration_info(getParam<bool>("output_iteration_info")),
    _output_iteration_info_on_error(getParam<bool>("output_iteration_info_on_error")),
    _relative_tolerance(parameters.get<Real>("relative_tolerance")),
    _absolute_tolerance(parameters.get<Real>("absolute_tolerance")),

    _ASR_extent(declareProperty<Real>("ASR_extent")),
    _ASR_extent_old(getMaterialPropertyOld<Real>("ASR_extent"))
{
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

  // stress dependent total ASR volumetric accounting for ASR gel adsorption due to tensile and
  // compressive cracking Eigen solve - Note the eigen values are ranked from minimum to maximum
  const Real sig_k = _stress_eigenvalues[2];

  Real gel_adsorption_tensile = 1.0;
  Real gel_adsorption_compress = 1.0;

  // Optionally calculate effect of ASR reaction on the tensile strength
  Real f_t = _f_tensile;
  if (_ASR_dependent_tensile_strength)
    f_t = _f_tensile * (1.0 - (1.0 - _beta_f_tensile) * _ASR_extent[_qp]);

  if (sig_k > _gamma_tensile * f_t)
    gel_adsorption_tensile =
        _gamma_residual + (1.0 - _gamma_residual) * (_gamma_tensile * f_t / sig_k);

  Real sig_effective = _stress[_qp].trace() / (3.0 * _f_compress);

  if (sig_effective > 0.0)
  {
    gel_adsorption_compress =
        1.0 - std::exp(_beta) * sig_effective / (1.0 + (std::exp(_beta) - 1.0) * sig_effective);
    if (gel_adsorption_compress > 1.0)
      gel_adsorption_compress = 1.0;
    if (gel_adsorption_compress < 0.0)
      gel_adsorption_compress = 0.0;
  }

  const Real inc_ASR_volumetric_strain = gel_adsorption_tensile * gel_adsorption_compress *
                                         std::pow(_rh[_qp], _rh_exponent) * inc_ASR_extent *
                                         _ASR_final_vstrain;

  return _volumetric_strain_old[_qp] + inc_ASR_volumetric_strain;
}

Real
ConcreteASREigenstrain::computeResidual(unsigned qp, Real scalar)
{
  Real f;
  if (_expansion == ExpansionType::Isotropic)
    f = 1.0;
  else if (_expansion == ExpansionType::Anisotropic)
  {
    const Real I_sigma = _stress[_qp].trace();
    if (I_sigma >= 0.0) // hydrostatic tension
      f = 1.0;
    else // hydrostatic compression: retarding ASR rection
    {
      f = 1.0 + _alpha * I_sigma / (3.0 * _f_compress);
      if (f < 1.0)
        mooseError("Wrong retardation for ASR latency time calculation!");
    }
  }
  else
    mooseError("Invalid expansion type");

  const Real T0 = _ASR_T0 + 273.15; // ASR reference temperature in Kelvin
  const Real T = _temperature[qp] + 273.15;

  // ASR characteristic and latency times (in days)
  Real tau_c = _tau_c_T0 * std::exp(_Uc * (1.0 / T - 1.0 / T0));
  Real tau_L = f * _tau_L_T0 * std::exp(_UL * (1.0 / T - 1.0 / T0));

  Real resid = scalar - _ASR_extent_old[qp] -
               _dt / 86400.0 / tau_c * (1.0 - scalar) / (1.0 + std::exp(-tau_L / tau_c)) *
                   (scalar + std::exp(-tau_L / tau_c));

  return resid;
}

Real
ConcreteASREigenstrain::computeDerivative(unsigned qp, Real scalar)
{
  Real f;
  if (_expansion == ExpansionType::Isotropic)
    f = 1.0;
  else if (_expansion == ExpansionType::Anisotropic)
  {
    const Real I_sigma = _stress[_qp].trace();
    if (I_sigma >= 0.0) // hydrostatic tension
      f = 1.0;
    else // hydrostatic compression: retarding ASR rection
    {
      f = 1.0 + _alpha * I_sigma / (3.0 * _f_compress);
      if (f < 1.0)
        mooseError("Wrong retardation for ASR latency time calculation!");
    }
  }
  else
    mooseError("Invalid expansion type");

  const Real T0 = _ASR_T0 + 273.15; // ASR reference temperature in Kelvin
  const Real T = _temperature[qp] + 273.15;

  // ASR characteristic and latency times (in days)
  Real tau_c = _tau_c_T0 * std::exp(_Uc * (1.0 / T - 1.0 / T0));
  Real tau_L = f * _tau_L_T0 * std::exp(_UL * (1.0 / T - 1.0 / T0));

  Real jac = 1.0 - _dt / 86400.0 / tau_c * (1.0 - scalar) / (1.0 + std::exp(-tau_L / tau_c)) -
             _dt / 86400.0 / tau_c * (-1) / (1.0 + std::exp(-tau_L / tau_c)) *
                 (scalar + std::exp(-tau_L / tau_c));
  return jac;
}
