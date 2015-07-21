/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                        Grizzly                               */
/*                                                              */
/*           (c) 2015 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "ConcreteElasticASRModel.h"

#include "SymmElasticityTensor.h"


template<>
InputParameters validParams<ConcreteElasticASRModel>()
{
  InputParameters params = validParams<ConstitutiveModel>();
  params.addCoupledVar("relative_humidity", "nonlinear variable name for rel. humidity");
  params.addParam<Real>("ASR_vol_expansion", 0.005, "final ansymptotic ASR volumertic expansion strain");
  params.addParam<Real>("tau_c_T0",   50.0, "chracteristic ASR time (in days) at reference temprature");
  params.addParam<Real>("tau_L_T0", 200.0, "Latency ASR time (in days) at reference temprature");
  params.addParam<Real>("Uc",       5400.0, "Activation energy associated with tau_c");
  params.addParam<Real>("UL",       9400.0, "Activation energy associated with tau_L");
  params.addParam<Real>("ASR_T0",   38.0,   "Reference temperature associated ASR in celcius");
  params.addParam<unsigned int>("max_its", 30, "Maximum number of sub-newton iterations");
  params.addParam<bool>("output_iteration_info", false, "Set true to output sub-newton iteration information");
  params.addParam<bool>("output_iteration_info_on_error", false, "Set true to output sub-newton iteration information when a step fails");
  params.addParam<Real>("relative_tolerance", 1e-5, "Relative convergence tolerance for sub-newtion iteration");
  params.addParam<Real>("absolute_tolerance", 1e-20, "Absolute convergence tolerance for sub-newtion iteration");



  return params;
}

ConcreteElasticASRModel::ConcreteElasticASRModel(const InputParameters & parameters)
    :ConstitutiveModel(parameters),
     _has_rh(isCoupled("relative_humidity")),
     _rh(_has_rh ? coupledValue("relative_humidity") : _zero),
     _ASR_final_vstrain(getParam<Real>("ASR_vol_expansion")),
     _tau_c_T0(getParam<Real>("tau_c_T0")),
     _tau_L_T0(getParam<Real>("tau_L_T0")),
     _Uc(getParam<Real>("Uc")),
     _UL(getParam<Real>("UL")),
     _ASR_T0(getParam<Real>("ASR_T0")),
     _max_its(parameters.get<unsigned int>("max_its")),
     _output_iteration_info(getParam<bool>("output_iteration_info")),
     _output_iteration_info_on_error(getParam<bool>("output_iteration_info_on_error")),
     _relative_tolerance(parameters.get<Real>("relative_tolerance")),
     _absolute_tolerance(parameters.get<Real>("absolute_tolerance")),

     _ASR_extent(declareProperty<Real>("ASR_extent")),
     _ASR_extent_old(declarePropertyOld<Real>("ASR_extent")),
     _ASR_volumetric_strain(declareProperty<Real>("ASR_strain")),
     _ASR_volumetric_strain_old(declarePropertyOld<Real>("ASR_strain"))
{
}

////////////////////////////////////////////////////////////////////////

ConcreteElasticASRModel::~ConcreteElasticASRModel()
{
}

////////////////////////////////////////////////////////////////////////
void
ConcreteElasticASRModel::initStatefulProperties(unsigned int n_points)
{
  for (unsigned qp(0); qp < n_points; ++qp)
  {
    _ASR_extent[qp]            = 0.0;
    _ASR_extent_old[qp]        = 0.0;
    _ASR_volumetric_strain[qp] = 0.0;
    _ASR_volumetric_strain_old[qp] = 0.0;
  }
}




void
ConcreteElasticASRModel::computeStress( const Elem & /*current_elem*/,
                             unsigned /*qp*/,
                             const SymmElasticityTensor & elasticity_tensor,
                             const SymmTensor & stress_old,
                             SymmTensor & strain_increment,
                             SymmTensor & stress_new )
{
  stress_new = elasticity_tensor * strain_increment;
  stress_new += stress_old;
}

bool
ConcreteElasticASRModel::modifyStrainIncrement(const Elem & /*elem*/,
                                     unsigned qp,
                                     SymmTensor & strain_increment,
                                     SymmTensor & d_strain_dT)
{
  bool modified_T = false;
  bool modified_ASR = false;
  // apply the thermal strain if needed
  modified_T = applyThermalStrain(qp, strain_increment, d_strain_dT);

  // compute incremental ASR volumetric strain
  modified_ASR = applyASRStrain(qp, strain_increment);
  return modified_T || modified_ASR;

}

bool
ConcreteElasticASRModel::applyASRStrain(unsigned qp, SymmTensor & strain_increment)
{

  // Use Newton sub-iteration to determine ASR reaction extent at new time step

  Real scalar = _ASR_extent_old[qp];
  unsigned int it = 0;
  Real residual = 10;
  Real norm_residual = 10;
  Real first_norm_residual = 10;

  std::stringstream iter_output;

  while (it < _max_its &&
        norm_residual > _absolute_tolerance &&
        (norm_residual/first_norm_residual) > _relative_tolerance)
  {
    residual = computeResidual(qp, scalar);
    norm_residual = std::abs(residual);
    if (it == 0)
    {
      first_norm_residual = norm_residual;
      if (first_norm_residual == 0)
      {
        first_norm_residual = 1;
      }
    }

    scalar -= residual / computeDerivative(qp, scalar);

    if (_output_iteration_info == true ||
        _output_iteration_info_on_error == true)
    {
      iter_output
        << " it="       << it
        << " ASR_extent="   << scalar
        << " rel_res="  << norm_residual/first_norm_residual
        << " rel_tol="  << _relative_tolerance
        << " abs_res="  << norm_residual
        << " abs_tol="  << _absolute_tolerance
        << std::endl;
    }

    ++it;
  }

  if (_output_iteration_info)
    _console << iter_output.str();
  if (it == _max_its &&
     norm_residual > _absolute_tolerance &&
     (norm_residual/first_norm_residual) > _relative_tolerance)
  {
    if (_output_iteration_info_on_error)
    {
      Moose::err << iter_output.str();
    }
    mooseError("Max sub-newton iteration hit during nonlinear constitutive model solve!");
  }

  // new ASR reaction extent and ASR volumetric strain
  _ASR_extent[qp] = scalar;

  _ASR_volumetric_strain[qp] =  _ASR_extent[qp] * _ASR_final_vstrain;

// we treat ASR expansion isotropically for now

  const Real oneThird = 1./3.;
//  const Real V0Vold = 1/_element->volumeRatioOld(_qp);
  const Real V0Vold = 1;


  const Real v_strain = std::pow(( _ASR_volumetric_strain[qp]    +1) * V0Vold, oneThird) -
                        std::pow(( _ASR_volumetric_strain_old[qp]+1) * V0Vold, oneThird);
  strain_increment.addDiag( -v_strain );


  bool modified = true;
  return modified;
}

Real
ConcreteElasticASRModel::computeResidual(unsigned qp,  Real scalar)
{
  Real T0= _ASR_T0 + 273.15; //ASR reference temperature in Kelvin

  Real T = T0;

  if (_has_temp)
    T= _temperature[qp] + 273.15;

  // ASR characteristic and latency times (in days)
  Real tau_c = _tau_c_T0 * std::exp(_Uc*(1/T - 1/T0));
  Real tau_L = _tau_L_T0 * std::exp(_UL*(1/T - 1/T0));

  Real extent_old = _ASR_extent_old[qp];

  Real Res = scalar - extent_old - _dt/86400.0/tau_c * (1-scalar)/(1+std::exp(-tau_L/tau_c)) * (scalar+std::exp(-tau_L/tau_c));

  return Res;
}

Real
ConcreteElasticASRModel::computeDerivative(unsigned qp,  Real scalar)
{
  Real T0= _ASR_T0 + 273.15; //ASR reference temperature in Kelvin

  Real T = T0;

  if (_has_temp)
    T= _temperature[qp] + 273.15;

  // ASR characteristic and latency times (in days)
  Real tau_c = _tau_c_T0 * std::exp(_Uc*(1/T - 1/T0));
  Real tau_L = _tau_L_T0 * std::exp(_UL*(1/T - 1/T0));

  Real jac = 1.0 - _dt/86400.0/tau_c * (1-scalar)/(1+std::exp(-tau_L/tau_c))
                 - _dt/86400.0/tau_c * (-1)/(1+std::exp(-tau_L/tau_c)) * (scalar+std::exp(-tau_L/tau_c)); ;
  return jac;
}

