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

#include "VSwellingASR.h"

#include "SolidModel.h"

/*////////////////////////////////
 *
 *note that this override of modifyStrain is for STRAIN RATE INCREMENTS
 *
 *//////////////////////////////

registerMooseObject("BlackBearApp", VSwellingASR);

template<>
InputParameters validParams<VSwellingASR>()
{
  InputParameters params = validParams<VolumetricModel>();

  MooseEnum ASR_formulation("isotropic anisotropic", "isotropic");
  params.addParam<MooseEnum>("ASR_formulation", ASR_formulation, "isotropic or anisotropic swelling, default: isotropic");
  params.addCoupledVar("temp", "Coupled Temperature");
  params.addCoupledVar("relative_humidity", "nonlinear variable name for rel. humidity");

  params.addParam<Real>("power_of_rh", 0.0, "power exponent of rel. humidity to ASR vol. strain");

  params.addParam<Real>("ASR_vol_expansion", 0.005, "final ansymptotic ASR volumertic expansion strain");
  params.addParam<Real>("tau_c_T0",   50.0, "chracteristic ASR time (in days) at reference temprature");
  params.addParam<Real>("tau_L_T0", 200.0, "Latency ASR time (in days) at reference temprature");
  params.addParam<Real>("Uc",       5400.0, "Activation energy associated with tau_c");
  params.addParam<Real>("UL",       9400.0, "Activation energy associated with tau_L");
  params.addParam<Real>("ASR_T0",   38.0,   "Reference temperature associated ASR in celcius");

  params.addParam<Real>("f_compress", -31.0e6,  "compressive strength of concrete (in Pascals)");
  params.addParam<Real>("alpha",        1.3333, "ASR latency time retardation constant under hydrostatic compression");
  params.addParam<Real>("f_tensile",    3.0e6,  "tensile strength of concrete (in Pascals)");
  params.addParam<Real>("f_u",        -10.0e6,  "upper compressive stress below which no ASR (in Pascals)");

  params.addParam<Real>("gamma_tensile",  0.5, "fraction of tensile strength beyond which ASR gel is adsorbed into tensile cracks");
  params.addParam<Real>("gamma_residual", 0.5, "Residual ASR retention factor under tension");

  params.addParam<Real>("beta", 0.5, "exponent for ASR retention factor under compressive stress state");

  params.addParam<bool>("ASR_dependent_tensile_strength", false, "Set true to turn ASR reaction dependent tensile strength");
  params.addParam<Real>("beta_f_tensile", 0.5, "fractional residual tensile strength at full ASR reaction");

  params.addParam<unsigned int>("max_its", 30, "Maximum number of sub-newton iterations");
  params.addParam<bool>("output_iteration_info", false, "Set true to output sub-newton iteration information");
  params.addParam<bool>("output_iteration_info_on_error", false, "Set true to output sub-newton iteration information when a step fails");
  params.addParam<Real>("relative_tolerance", 1e-5, "Relative convergence tolerance for sub-newtion iteration");
  params.addParam<Real>("absolute_tolerance", 1e-20, "Absolute convergence tolerance for sub-newtion iteration");

  return params;
}

VSwellingASR::VSwellingASR(const InputParameters & parameters)
  : VolumetricModel(parameters),
    _has_temp(isCoupled("temp")),
    _temperature(_has_temp ? coupledValue("temp") : _zero),
    _temperature_old(_has_temp ? coupledValueOld("temp") : _zero),

    _has_rh(isCoupled("relative_humidity")),
    _rh(_has_rh ? coupledValue("relative_humidity") : _zero),
    _m_power(getParam<Real>("power_of_rh")),

    _ASR_formulation(getParam<MooseEnum>("ASR_formulation")),

    _ASR_final_vstrain(getParam<Real>("ASR_vol_expansion")),
    _tau_c_T0(getParam<Real>("tau_c_T0")),
    _tau_L_T0(getParam<Real>("tau_L_T0")),
    _Uc(getParam<Real>("Uc")),
    _UL(getParam<Real>("UL")),
    _ASR_T0(getParam<Real>("ASR_T0")),
    _f_compress(getParam<Real>("f_compress")),
    _alpha(getParam<Real>("alpha")),
    _f_tensile(getParam<Real>("f_tensile")),
    _f_u(getParam<Real>("f_u")),

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
    _ASR_extent_old(getMaterialPropertyOld<Real>("ASR_extent")),
    _ASR_volumetric_strain(declareProperty<Real>("ASR_vol_strain")),
    _ASR_volumetric_strain_old(getMaterialPropertyOld<Real>("ASR_vol_strain")),
    _ASR_strain(declareProperty<SymmTensor>("ASR_strain")),
    _ASR_strain_old(getMaterialPropertyOld<SymmTensor>("ASR_strain")),
    _stress_prop(getMaterialPropertyOld<SymmTensor>("stress"))
{
}

void
VSwellingASR::initStatefulProperties(unsigned n_points)
{
  for (unsigned int qp = 0; qp < n_points; ++qp)
  {
    _ASR_extent[qp]            = 0.0;
    _ASR_volumetric_strain[qp] = 0.0;

    SymmTensor a(0.0);

    _ASR_strain[qp] = a;
  }
}

void
VSwellingASR::modifyStrain(const unsigned int qp,
                           const Real v0OverVOld,
                           SymmTensor & strain_increment,
                           SymmTensor & /*dstrain_increment_dT*/)
{
  // compute incremental ASR volumetric strain
  applyASRStrain(qp, v0OverVOld, strain_increment);
}


bool
VSwellingASR::applyASRStrain(unsigned qp, const Real v0OverVOld, SymmTensor & strain_increment)
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
         (norm_residual / first_norm_residual) > _relative_tolerance)
  {
    residual = computeResidual(qp, scalar);
    norm_residual = std::abs(residual);
    if (it == 0)
    {
      first_norm_residual = norm_residual;
      if (first_norm_residual == 0)
        first_norm_residual = 1;
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
      Moose::err << iter_output.str();
    mooseError("Max sub-newton iteration hit during nonlinear constitutive model solve!");
  }

  // new ASR reaction extent
  _ASR_extent[qp] = scalar;
  Real inc_ASR_extent = _ASR_extent[qp] - _ASR_extent_old[qp];

  // new total volumetric strain
  Real rh_value = 1.0;
  if (_has_rh)
    rh_value = _rh[qp];

  // stress dependant total ASR volumetric accounting for ASR gel adsorption due to tensile and compressive cracking
  SymmTensor stress(_stress_prop[qp]);
  // Column major matrix for stress
  ColumnMajorMatrix stress_cm = stress.columnMajorMatrix();

  // Eigen solve - Note the eigen values are ranked from minimum to maximum
  const int ND = 3;
  ColumnMajorMatrix eval(ND, 1);
  ColumnMajorMatrix evec(ND, ND);
  stress_cm.eigen(eval, evec);

  Real sig_l = eval(0, 0);
  Real sig_m = eval(1, 0);
  Real sig_k = eval(2, 0);

  Real gel_adsorption_tensile  = 1.0;
  Real gel_adsorption_compress = 1.0;

  // calculate ASR reaction to the tensile strength
  Real f_t = _f_tensile;
  if (_ASR_dependent_tensile_strength)
    f_t = _f_tensile * (1.0 - (1.0 - _beta_f_tensile) * _ASR_extent[qp]);

  if (sig_k > _gamma_tensile * f_t)
    gel_adsorption_tensile = _gamma_residual + (1.0 - _gamma_residual) * (_gamma_tensile * f_t / sig_k);

  Real I_sigma = stress.trace();
  Real sig_effective = I_sigma / (3.0 * _f_compress);

  if (sig_effective > 0.0)
  {
    gel_adsorption_compress = 1.0 - std::exp(_beta) * sig_effective / (1.0 + (std::exp(_beta)-1.0) * sig_effective);
    if (gel_adsorption_compress > 1.0) gel_adsorption_compress = 1.0;
    if (gel_adsorption_compress < 0.0) gel_adsorption_compress = 0.0;
  }

  Real inc_ASR_volumetric_strain = gel_adsorption_tensile *  gel_adsorption_compress * std::pow(rh_value, _m_power) * inc_ASR_extent * _ASR_final_vstrain;
  _ASR_volumetric_strain[qp] = _ASR_volumetric_strain_old[qp] + inc_ASR_volumetric_strain;

  Real v_strain = 0.0;
  switch (_ASR_formulation)
  {
    case 0: // isotropic swellling
      v_strain = std::pow(( _ASR_volumetric_strain[qp]    +1) * v0OverVOld, 1.0/3.0) -
                          std::pow(( _ASR_volumetric_strain_old[qp]+1) * v0OverVOld, 1.0/3.0);
      strain_increment.addDiag( -v_strain );
      _ASR_strain[qp] = _ASR_strain_old[qp];
      _ASR_strain[qp].addDiag(v_strain);

      break;

    case 1: // anisotropic swelling
    {
      Real sig_u = _f_u;

      Real f_c = _f_compress;
      Real W_1;
      Real W_2;
      Real W_3;

      SymmTensor _inc_weighted_ASR_strain_principal;
      SymmTensor _inc_weighted_ASR_strain_cartesian;

      // Weights
      // W_k
      W_3 = Weight(sig_l, sig_m,  sig_k, sig_u, f_c, f_t);

      // W_m
      W_2 = Weight(sig_k, sig_l,  sig_m, sig_u, f_c, f_t);

      //W_l
      W_1 = Weight(sig_m, sig_k,  sig_l, sig_u, f_c, f_t);

      // we treat ASR expansion anisotropically this time
      _inc_weighted_ASR_strain_principal(0, 0) = inc_ASR_volumetric_strain * W_1;
      _inc_weighted_ASR_strain_principal(1, 1) = inc_ASR_volumetric_strain * W_2;
      _inc_weighted_ASR_strain_principal(2, 2) = inc_ASR_volumetric_strain * W_3;

      // Rotate back to Catesian coordnate system
      SolidModel::rotateSymmetricTensor(evec, _inc_weighted_ASR_strain_principal, _inc_weighted_ASR_strain_cartesian);

      // Should I add '-' sign somewhere?
      strain_increment -= _inc_weighted_ASR_strain_cartesian;
      _ASR_strain[qp] = _ASR_strain_old[qp] + _inc_weighted_ASR_strain_cartesian;

      break;
    }

    default:mooseError("Unknown ASR swelling model");
      break;
  }


  bool modified = true;
  return modified;
}

Real
VSwellingASR::computeResidual(unsigned qp,  Real scalar)
{
  SymmTensor stress(_stress_prop[qp]);
  Real I_sigma = stress.trace();
  Real f;
  switch (_ASR_formulation)
  {
    case 0: // isotropic swellling
      f = 1.0;
      break;

    case 1: // anisotropic swelling
      if (I_sigma >= 0.0) // hydrostatic tension
        f = 1.0;
      else //hydrostatic compression: retarding ASR rection
      {
        f = 1.0 + _alpha * I_sigma/(3.0 * _f_compress);
        if (f < 1.0)
          mooseError("Wrong retardation for ASR latency time calculation!");
      }
      break;

    default:mooseError("Unknown ASR swelling model");
      break;
  }

  Real T0= _ASR_T0 + 273.15; //ASR reference temperature in Kelvin
  Real T = T0;
  if (_has_temp)
    T= _temperature[qp] + 273.15;

  // ASR characteristic and latency times (in days)
  Real tau_c = _tau_c_T0 * std::exp(_Uc * (1.0 / T - 1.0 / T0));
  Real tau_L = f * _tau_L_T0 * std::exp(_UL * (1.0 / T - 1.0 / T0));

  Real extent_old = _ASR_extent_old[qp];
  Real Res = scalar - extent_old - _dt / 86400.0 / tau_c * (1.0 - scalar) / (1.0 + std::exp(-tau_L / tau_c)) * (scalar + std::exp(-tau_L/tau_c));

  return Res;
}

Real
VSwellingASR::computeDerivative(unsigned qp, Real scalar)
{
  SymmTensor stress(_stress_prop[qp]);
  Real I_sigma = stress.trace();
  Real f;
  switch (_ASR_formulation)
  {
    case 0: // isotropic swellling
      f = 1.0;
      break;

    case 1: // anisotropic swelling
      if (I_sigma >= 0.0) // hydrostatic tension
        f = 1.0;
      else //hydrostatic compression: retarding ASR rection
      {
        f = 1.0 + _alpha * I_sigma/(3.0 * _f_compress);
        if (f < 1.0)
          mooseError("Wrong retardation for ASR latency time calculation!");
      }
      break;

    default:mooseError("Unknown ASR swelling model");
      break;
  }

  Real T0= _ASR_T0 + 273.15; //ASR reference temperature in Kelvin
  Real T = T0;
  if (_has_temp)
    T= _temperature[qp] + 273.15;

  // ASR characteristic and latency times (in days)
  Real tau_c = _tau_c_T0 * std::exp(_Uc*(1/T - 1/T0));
  Real tau_L = f * _tau_L_T0 * std::exp(_UL*(1/T - 1/T0));

  Real jac = 1.0 - _dt/86400.0/tau_c * (1-scalar)/(1+std::exp(-tau_L/tau_c))
                 - _dt/86400.0/tau_c * (-1)/(1+std::exp(-tau_L/tau_c)) * (scalar+std::exp(-tau_L/tau_c)); ;
  return jac;
}

Real
VSwellingASR::Weight(Real sig_l, Real sig_m, Real sig_k, Real sig_u, Real f_c, Real f_t)
{
  // Inputs
  Real a1 = f_t, a2 = sig_u, a3 = f_c - sig_u;
  Real b1 = f_t, b2 = sig_u, b3 = f_c - sig_u;
  Real a, b;
  Real W;

  // Declaration
  int pbound_l, pbound_m, pbound_k;
  int N1, N2, N3, N4, N5, N6;

  // Lower and upper bound for l
  pbound_l = Find_neighbors(sig_l, f_c, sig_u, f_t);

  // Lower and upper bound for m
  pbound_m = Find_neighbors(sig_m, f_c, sig_u, f_t);

  // Neighboring nodes of k
  pbound_k = Find_neighbors(sig_k, f_c, sig_u, f_t);

  // Neighboring nodes of l, m, k
  N1 = (pbound_m)*4 + pbound_l;
  N2 = (pbound_m)*4 + (pbound_l + 1);
  N3 = (pbound_m + 1)*4 + pbound_l;
  N4 = (pbound_m + 1)*4 + (pbound_l + 1);

  N5 = pbound_k;
  N6 = pbound_k + 1;

  // Calculate a, b, sig_l, sig_m
  a = Cal_ab(a1, a2, a3, pbound_l);
  b = Cal_ab(b1, b2, b3, pbound_m);
  sig_l = Cal_sig(sig_l, sig_u, pbound_l, f_c, f_t);
  sig_m = Cal_sig(sig_m, sig_u, pbound_m, f_c, f_t);

  // Calculate weights
  W = Cal_W(N1, N2, N3, N4, N5, N6, a, b, sig_l, sig_m, sig_k, f_t, sig_u, f_c);

  return W;
}


int
VSwellingASR::Find_neighbors(Real sig, Real /*f_c*/, Real sig_u, Real /*f_t*/)
{
  if (sig <= sig_u)
    return 2;
  else if (sig > sig_u && sig <= 0)
    return 1;
  else if (sig > 0)
    return 0;
  else
    mooseError("Invalid value for sig.");
}

Real
VSwellingASR::Cal_ab(Real ab1, Real ab2, Real ab3, int pbound)
{
  Real ab;
  if (pbound == 0)
    ab = ab1;
  else if (pbound == 1)
    ab = ab2;
  else if (pbound == 2)
    ab = ab3;
  else
    mooseError("Invalid value for pbound. Got ", pbound, ", expected 0, 1, or 2.");

  return ab;
}

Real
VSwellingASR::Cal_sig(Real sig, Real sig_u, int pbound, Real f_c, Real f_t)
{
  if (pbound == 2)
  {
    if (sig  < f_c)       // stress conditions beyond f_c
      return f_c - sig_u;
    else
      return sig - sig_u;
  }
  else
  {
    if (sig > f_t)
      return f_t;         // stress conditions beyond f_t
    else
      return sig;
  }
}

Real
VSwellingASR::Cal_W(int N1, int N2, int N3, int N4, int N5, int N6, Real a, Real b, Real sig_l, Real sig_m, Real sig_k, Real f_t, Real sig_u, Real f_c)
{
  Real W1, W2, W3, W4, WN1, WN2, WN3, WN4, W;
  // Table initiation
  std::vector<std::vector<Real> > Table(16);
  for (unsigned int i = 0; i < 16; ++i)
    Table[i].resize(4);

  // Triaxial weight table
  Real Tablearr[16][4] =
  {
    {1.0/3, 1.0/3, 0, 0},   //  0 -> 13
    {1.0/3, 1.0/3, 0, 0},   //  1 -> 14
    {1.0/2, 1.0/2, 0, 0},   //  2 -> 15
    {1.0/2, 1.0/2, 0, 0},   //  3 -> 16
    {1.0/3, 1.0/3, 0, 0},   //  4 -> 12
    {1.0/3, 1.0/3, 0, 0},   //  5 ->  1
    {1.0/2, 1.0/2, 0, 0},   //  6 ->  2
    {1.0/2, 1.0/2, 0, 0},   //  7 ->  5
    {1.0/2, 1.0/2, 0, 0},   //  8 -> 11
    {1.0/2, 1.0/2, 0, 0},   //  9 ->  4
    {1.0, 1.0, 1.0/3, 0},   // 10 ->  3
    {1.0, 1.0, 1.0/2, 0},   // 11 ->  6
    {1.0/2, 1.0/2, 0, 0},   // 12 -> 10
    {1.0/2, 1.0/2, 0, 0},   // 13 ->  9
    {1.0, 1.0, 1.0/2, 0},   // 14 ->  8
    {1.0, 1.0, 1.0, 1.0/3}, // 15 ->  7
  };

  // Table assignment
  for (unsigned int i = 0; i < 16; ++i)
    for (unsigned int j = 0; j < 4; ++j)
      Table[i][j] = Tablearr[i][j];

  if (sig_k < f_c)
    sig_k = f_c;
  else if (sig_k > f_t)
    sig_k = f_t;

  // Calcualte weights
  // N1
  W1 = W_i(N1, N5, N6, sig_k, Table, f_t, sig_u, f_c);
  // N2
  W2 = W_i(N2, N5, N6, sig_k, Table, f_t, sig_u, f_c);
  // N3
  W3 = W_i(N3, N5, N6, sig_k, Table, f_t, sig_u, f_c);
  // N4
  W4 = W_i(N4, N5, N6, sig_k, Table, f_t, sig_u, f_c);

  //   local node numebers
  //   3-----4
  //   |     |
  //   |     |
  //   1-----2
  WN1 = (1.0/(a*b))*(a-sig_l)*(b-sig_m);
  WN2 = (1.0/(a*b))*sig_l*(b-sig_m);
  WN3 = (1.0/(a*b))*(a-sig_l)*sig_m;
  WN4 = (1.0/(a*b))*sig_l*sig_m;

  W = W1*WN1 + W2*WN2 + W3*WN3 + W4*WN4;
  return W;
}

Real
VSwellingASR::W_i(int N, int N5, int N6, Real sig_k, std::vector<std::vector<Real> > & Table, Real f_t, Real sig_u, Real f_c)
{
  Real W_1, W_2, W;
  Real farr[4] = {f_t, 0, sig_u, f_c};
  W_1 = Table[N][N5];
  W_2 = Table[N][N6];
  W = W_1 + (W_2-W_1)*(sig_k-farr[N5])/(farr[N6]-farr[N5]);

  return W;
}
