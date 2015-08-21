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

#include "ConcreteElasticASR.h"

template<>
InputParameters validParams<ConcreteElasticASR>()
{
  InputParameters params = validParams<SolidModel>();
  params.addCoupledVar("relative_humidity", "nonlinear variable name for rel. humidity");
  params.addParam<Real>("ASR_vol_expansion", 0.005, "final ansymptotic ASR volumertic expansion strain");
  params.addParam<Real>("tau_c_T0",   50.0, "characteristic ASR time (in days) at reference temperature");
  params.addParam<Real>("tau_L_T0", 200.0, "Latency ASR time (in days) at reference temperature");
  params.addParam<Real>("Uc",       5400.0, "Activation energy associated with tau_c");
  params.addParam<Real>("UL",       9400.0, "Activation energy associated with tau_L");
  params.addParam<Real>("ASR_T0",   38.0,   "Reference temperature associated ASR in celcius");
  params.addParam<unsigned int>("max_its", 30, "Maximum number of sub-newton iterations");
  params.addParam<bool>("output_iteration_info", false, "Set true to output sub-newton iteration information");
  params.addParam<bool>("output_iteration_info_on_error", false, "Set true to output sub-newton iteration information when a step fails");
  params.addParam<Real>("relative_tolerance", 1e-5, "Relative convergence tolerance for sub-newton iteration");
  params.addParam<Real>("absolute_tolerance", 1e-20, "Absolute convergence tolerance for sub-newton iteration");

  return params;
}

ConcreteElasticASR::ConcreteElasticASR(const InputParameters & parameters)
  :SolidModel(parameters)
{

  createConstitutiveModel("ConcreteElasticASRModel");

}

////////////////////////////////////////////////////////////////////////

ConcreteElasticASR::~ConcreteElasticASR()
{
}

////////////////////////////////////////////////////////////////////////
