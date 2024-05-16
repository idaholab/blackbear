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
#include "DamagePlasticityStressUpdate.h"
#include "libmesh/utility.h"

registerMooseObject("BlackBearApp", DamagePlasticityStressUpdate);

InputParameters
DamagePlasticityStressUpdate::validParams()
{
  InputParameters params = MultiParameterPlasticityStressUpdate::validParams();
  params.addParam<Real>(
      "yield_function_tolerance",
      "If the yield function is less than this amount, the (stress, internal parameters) are "
      "deemed admissible.  A std::vector of tolerances must be entered for the multi-surface case");
  params.addRangeCheckedParam<Real>("biaxial_uniaxial_compressive_stress_factor",
                                    0.1,
                                    "biaxial_uniaxial_compressive_stress_factor < 0.5 & "
                                    "biaxial_uniaxial_compressive_stress_factor >= 0",
                                    "Material parameter that relate biaxial and uniaxial "
                                    "compressive  strength, i.e., \alfa = (fb0-fc0)/(2*fb0-fc0)");
  params.addRequiredParam<Real>("dilatancy_factor", "controls the dilation of concrete");
  params.addRangeCheckedParam<Real>("stiff_recovery_factor",
                                    0.,
                                    "stiff_recovery_factor <= 1. & stiff_recovery_factor >= 0",
                                    "stiffness recovery factor");
  params.addRangeCheckedParam<Real>(
      "ft_ep_slope_factor_at_zero_ep",
      "ft_ep_slope_factor_at_zero_ep <= 1 & ft_ep_slope_factor_at_zero_ep >= 0",
      "slope of ft vs plastic strain curve at zero plastic strain");

  params.addRequiredParam<Real>(
      "tensile_damage_at_half_tensile_strength",
      "fraction of the elastic recovery slope in tension at 0.5*ft0 after yielding");
  params.addRangeCheckedParam<Real>("yield_strength_in_tension",
                                    "yield_strength_in_tension >= 0",
                                    "Tensile yield strength of concrete");
  params.addRangeCheckedParam<Real>("fracture_energy_in_tension",
                                    "fracture_energy_in_tension >= 0",
                                    "Fracture energy of concrete in uniaxial tension");
  params.addRangeCheckedParam<Real>("yield_strength_in_compression",
                                    "yield_strength_in_compression >= 0",
                                    "Absolute yield compressice strength");
  params.addRequiredParam<Real>("compressive_damage_at_max_compressive_strength",
                                "damage at maximum compressive strength");
  params.addRequiredParam<Real>("maximum_strength_in_compression",
                                "Absolute maximum compressive strength");
  params.addRangeCheckedParam<Real>("fracture_energy_in_compression",
                                    "fracture_energy_in_compression >= 0",
                                    "Fracture energy of concrete in uniaxial compression");

  params.addRequiredRangeCheckedParam<Real>(
      "tip_smoother",
      "tip_smoother>=0",
      "Smoothing parameter: the cone vertex at mean = cohesion*cot(friction_angle), will be "
      "smoothed by the given amount. Typical value is 0.1*cohesion");
  params.addParam<bool>("perfect_guess",
                        true,
                        "Provide a guess to the Newton-Raphson proceedure "
                        "that is the result from perfect plasticity.  With "
                        "severe hardening/softening this may be "
                        "suboptimal.");
  params.addClassDescription("Damage Plasticity Model for concrete");
  return params;
}

DamagePlasticityStressUpdate::DamagePlasticityStressUpdate(const InputParameters & parameters)
  : MultiParameterPlasticityStressUpdate(parameters, 3, 1, 2),
    _f_tol(getParam<Real>("yield_function_tol")),

    _alfa(getParam<Real>("biaxial_uniaxial_compressive_stress_factor")),
    _alfa_p(getParam<Real>("dilatancy_factor")),
    _s0(getParam<Real>("stiff_recovery_factor")),

    _Chi(getParam<Real>("ft_ep_slope_factor_at_zero_ep")),
    _Dt(getParam<Real>("tensile_damage_at_half_tensile_strength")),
    _ft(getParam<Real>("yield_strength_in_tension")),
    _FEt(getParam<Real>("fracture_energy_in_tension")),
    _fyc(getParam<Real>("yield_strength_in_compression")),
    _Dc(getParam<Real>("compressive_damage_at_max_compressive_strength")),
    _fc(getParam<Real>("maximum_strength_in_compression")),
    _FEc(getParam<Real>("fracture_energy_in_compression")),

    _ft0(_ft),
    _fc0(_fyc),
    _at(1.5 * std::sqrt(1 - _Chi) - 0.5),
    _ac((2. * (_fc / _fyc) - 1. + 2. * std::sqrt(Utility::pow<2>(_fc / _fyc) - _fc / _fyc))),
    _zt((1. + _at) / _at),
    _zc((1. + _ac) / _ac),
    _dt_bt(log(1. - _Dt) / log((1. + _at - std::sqrt(1. + _at * _at)) / (2. * _at))),
    _dc_bc(log(1. - _Dc) / log((1. + _ac) / (2. * _ac))),

    _small_smoother2(Utility::pow<2>(getParam<Real>("tip_smoother"))),

    _sqrt3(std::sqrt(3.)),
    _perfect_guess(getParam<bool>("perfect_guess")),
    _eigvecs(RankTwoTensor()),
    _intnl0(declareProperty<Real>("damage_state_in_tension")),
    _intnl1(declareProperty<Real>("damage_state_in_compression")),
    _ele_len(declareProperty<Real>("element_length")),
    _gt(declareProperty<Real>("fracture_energy_in_tension")),
    _gc(declareProperty<Real>("fracture_energy_in_compression")),
    _tD(declareProperty<Real>("tensile_damage")),
    _cD(declareProperty<Real>("compression_damage")),
    _D(declareProperty<Real>("damage_variable")),
    _min_ep(declareProperty<Real>("min_ep")),
    _mid_ep(declareProperty<Real>("mid_ep")),
    _max_ep(declareProperty<Real>("max_ep")),
    _sigma0(declareProperty<Real>("damaged_min_principal_stress")),
    _sigma1(declareProperty<Real>("damaged_mid_principal_stress")),
    _sigma2(declareProperty<Real>("damaged_max_principal_stress"))
{
}

void
DamagePlasticityStressUpdate::initQpStatefulProperties()
{
  /* There are multiple ways to determine the correct element length (or the characteristic length)
      used, the following commented lines show several different options. Some other options are
      still being considered. In this code, we define the element length as the cube root of the
      element volume */

  // if (_current_elem->n_vertices() < 3)
  //   _ele_len[_qp] = _current_elem->length(0, 1);
  // else if (_current_elem->n_vertices() < 5)
  //   _ele_len[_qp] = (_current_elem->length(0, 1) + _current_elem->length(1, 2)) / 2.;
  // else
  //   _ele_len[_qp] =
  //       (_current_elem->length(0, 1) + _current_elem->length(1, 2) + _current_elem->length(0, 4))
  //       / 3.;
  _ele_len[_qp] = std::cbrt(_current_elem->volume());

  _gt[_qp] = _FEt / _ele_len[_qp];
  _gc[_qp] = _FEc / _ele_len[_qp];

  _min_ep[_qp] = 0.;
  _mid_ep[_qp] = 0.;
  _max_ep[_qp] = 0.;
  _sigma0[_qp] = 0.;
  _sigma1[_qp] = 0.;
  _sigma2[_qp] = 0.;
  _intnl0[_qp] = 0.;
  _intnl1[_qp] = 0.;
  _tD[_qp] = 0.;
  _cD[_qp] = 0.;
  _D[_qp] = 0.;
  MultiParameterPlasticityStressUpdate::initQpStatefulProperties();
}

void
DamagePlasticityStressUpdate::finalizeReturnProcess(const RankTwoTensor & /*rotation_increment*/)
{
  std::vector<Real> eigstrain;
  _plastic_strain[_qp].symmetricEigenvalues(eigstrain);
  _min_ep[_qp] = eigstrain[0];
  _mid_ep[_qp] = eigstrain[1];
  _max_ep[_qp] = eigstrain[2];
}

void
DamagePlasticityStressUpdate::computeStressParams(const RankTwoTensor & stress,
                                                  std::vector<Real> & stress_params) const
{
  stress.symmetricEigenvalues(stress_params);
}

std::vector<RankTwoTensor>
DamagePlasticityStressUpdate::dstress_param_dstress(const RankTwoTensor & stress) const
{
  std::vector<Real> sp;
  std::vector<RankTwoTensor> dsp;
  stress.dsymmetricEigenvalues(sp, dsp);
  return dsp;
}

std::vector<RankFourTensor>
DamagePlasticityStressUpdate::d2stress_param_dstress(const RankTwoTensor & stress) const
{
  std::vector<RankFourTensor> d2;
  stress.d2symmetricEigenvalues(d2);
  return d2;
}

void
DamagePlasticityStressUpdate::setEffectiveElasticity(const RankFourTensor & Eijkl)
{
  // Eijkl is required to be isotropic, so we can use the
  // frame where stress is diagonal
  for (unsigned a = 0; a < _num_sp; ++a)
    for (unsigned b = 0; b < _num_sp; ++b)
      _Eij[a][b] = Eijkl(a, a, b, b);
  _En = _Eij[2][2];
  const Real denom = _Eij[0][0] * (_Eij[0][0] + _Eij[0][1]) - 2 * Utility::pow<2>(_Eij[0][1]);
  for (unsigned a = 0; a < _num_sp; ++a)
  {
    _Cij[a][a] = (_Eij[0][0] + _Eij[0][1]) / denom;
    for (unsigned b = 0; b < a; ++b)
      _Cij[a][b] = _Cij[b][a] = -_Eij[0][1] / denom;
  }
}

void
DamagePlasticityStressUpdate::preReturnMapV(const std::vector<Real> & /*trial_stress_params*/,
                                            const RankTwoTensor & stress_trial,
                                            const std::vector<Real> & /*intnl_old*/,
                                            const std::vector<Real> & /*yf*/,
                                            const RankFourTensor & /*Eijkl*/)
{
  std::vector<Real> eigvals;
  stress_trial.symmetricEigenvaluesEigenvectors(eigvals, _eigvecs);
}

void
DamagePlasticityStressUpdate::setStressAfterReturnV(const RankTwoTensor & /*stress_trial*/,
                                                    const std::vector<Real> & stress_params,
                                                    Real /*gaE*/,
                                                    const std::vector<Real> & intnl,
                                                    const yieldAndFlow & /*smoothed_q*/,
                                                    const RankFourTensor & /*Eijkl*/,
                                                    RankTwoTensor & stress) const
{
  // form the diagonal stress
  stress = RankTwoTensor(stress_params[0], stress_params[1], stress_params[2], 0.0, 0.0, 0.0);
  // rotate to the original frame
  stress = _eigvecs * stress * (_eigvecs.transpose());
  //   _dir[_qp] = _eigvecs;
  Real D = damageVar(stress_params, intnl);
  _sigma0[_qp] = (1. - D) * stress_params[0];
  _sigma1[_qp] = (1. - D) * stress_params[1];
  _sigma2[_qp] = (1. - D) * stress_params[2];
  _intnl0[_qp] = intnl[0];
  _intnl1[_qp] = intnl[1];
  _D[_qp] = D;
}

void
DamagePlasticityStressUpdate::yieldFunctionValuesV(const std::vector<Real> & stress_params,
                                                   const std::vector<Real> & intnl,
                                                   std::vector<Real> & yf) const
{
  Real I1 = stress_params[0] + stress_params[1] + stress_params[2];
  Real J2 = RankTwoTensor(stress_params[0], stress_params[1], stress_params[2], 0, 0, 0)
                .secondInvariant();
  yf[0] = 1. / (1. - _alfa) *
              (_alfa * I1 + _sqrt3 * std::sqrt(J2) +
               beta(intnl) * (stress_params[2] < 0. ? 0. : stress_params[2])) -
          fbar(_fc0, _ac, 1. - _dc_bc, intnl[1]);
}

void
DamagePlasticityStressUpdate::computeAllQV(const std::vector<Real> & stress_params,
                                           const std::vector<Real> & intnl,
                                           std::vector<yieldAndFlow> & all_q) const
{
  Real I1 = stress_params[0] + stress_params[1] + stress_params[2];
  Real J2 = RankTwoTensor(stress_params[0], stress_params[1], stress_params[2], 0, 0, 0)
                .secondInvariant();
  RankTwoTensor d_sqrt_3J2;

  Real fcbar, dfcbar;
  fcbar = fbar(_fc0, _ac, 1. - _dc_bc, intnl[1]);
  dfcbar = dfbar_dkappa(_fc0, _ac, 1. - _dc_bc, intnl[1]);

  if (J2 == 0)
    d_sqrt_3J2 = RankTwoTensor();
  else
    d_sqrt_3J2 = 0.5 * std::sqrt(3.0 / J2) *
                 RankTwoTensor(stress_params[0], stress_params[1], stress_params[2], 0, 0, 0)
                     .dsecondInvariant();

  all_q[0].f = 1. / (1. - _alfa) *
                   (_alfa * I1 + _sqrt3 * std::sqrt(J2) +
                    beta(intnl) * (stress_params[2] < 0. ? 0. : stress_params[2])) -
               fcbar;

  for (unsigned i = 0; i < _num_sp; ++i)
    if (J2 == 0)
      all_q[0].df[i] =
          1. / (1. - _alfa) * (_alfa + beta(intnl) * (stress_params[2] < 0. ? 0. : (i == 2)));
    else
      all_q[0].df[i] =
          1. / (1. - _alfa) *
          (_alfa + d_sqrt_3J2(i, i) + beta(intnl) * (stress_params[2] < 0. ? 0. : (i == 2)));
  all_q[0].df_di[0] =
      1. / (1. - _alfa) * (dbeta0(intnl) * (stress_params[2] < 0. ? 0. : stress_params[2]));
  all_q[0].df_di[1] =
      1. / (1. - _alfa) * (dbeta1(intnl) * (stress_params[2] < 0. ? 0. : stress_params[2])) -
      dfcbar;

  flowPotential(stress_params, intnl, all_q[0].dg);
  dflowPotential_dstress(stress_params, intnl, all_q[0].d2g);
  dflowPotential_dintnl(stress_params, intnl, all_q[0].d2g_di);
}

void
DamagePlasticityStressUpdate::flowPotential(const std::vector<Real> & stress_params,
                                            const std::vector<Real> & /* intnl */,
                                            std::vector<Real> & r) const
{
  Real J2 = RankTwoTensor(stress_params[0], stress_params[1], stress_params[2], 0, 0, 0)
                .secondInvariant();
  RankTwoTensor d_sqrt_2J2;
  if (J2 == 0)
    d_sqrt_2J2 = RankTwoTensor();
  else
    d_sqrt_2J2 = 0.5 * std::sqrt(2.0 / J2) *
                 RankTwoTensor(stress_params[0], stress_params[1], stress_params[2], 0, 0, 0)
                     .dsecondInvariant();

  for (unsigned int i = 0; i < _num_sp; ++i)
    r[i] = (_alfa_p + d_sqrt_2J2(i, i));
}

void
DamagePlasticityStressUpdate::dflowPotential_dstress(
    const std::vector<Real> & stress_params,
    const std::vector<Real> & /* intnl */,
    std::vector<std::vector<Real>> & dr_dstress) const
{
  Real J2 = RankTwoTensor(stress_params[0], stress_params[1], stress_params[2], 0, 0, 0)
                .secondInvariant();
  RankTwoTensor dII = RankTwoTensor(stress_params[0], stress_params[1], stress_params[2], 0, 0, 0)
                          .dsecondInvariant();
  RankTwoTensor d_sqrt_2J2;
  RankFourTensor dfp;
  RankTwoTensor d2J2_dsigi_dsigj =
      RankTwoTensor(2. / 3., 2. / 3., 2. / 3., -1. / 3., -1. / 3., -1. / 3.);
  std::vector<Real> dJ2_dsigi(3);
  for (unsigned i = 0; i < 3; ++i)
    dJ2_dsigi[i] =
        (2 * stress_params[i] - stress_params[(i + 1) % 3] - stress_params[(i + 2) % 3]) / 3;

  if (J2 == 0)
  {
    for (unsigned i = 0; i < 3; ++i)
      for (unsigned j = 0; j < 3; ++j)
        dr_dstress[i][j] = 0.0;

  }
  else
  {
    for (unsigned i = 0; i < 3; ++i)
      for (unsigned j = 0; j < 3; ++j)
        dr_dstress[i][j] = 0.5 * (std::sqrt(2.0 / J2) * d2J2_dsigi_dsigj(i, j) -
                            (1 / std::sqrt(2)) * std::pow(J2, -1.5) * dJ2_dsigi[i]*dJ2_dsigi[j]);
  }
}

void
DamagePlasticityStressUpdate::dflowPotential_dintnl(
    const std::vector<Real> & /* stress_params */,
    const std::vector<Real> & /* intnl */,
    std::vector<std::vector<Real>> & dr_dintnl) const
{
  for (unsigned i = 0; i < _num_sp; ++i)
    for (unsigned j = 0; j < _num_intnl; ++j)
      dr_dintnl[i][j] = 0.;
}

void
DamagePlasticityStressUpdate::hardPotential(const std::vector<Real> & stress_params,
                                            const std::vector<Real> & intnl,
                                            std::vector<Real> & h) const
{
  Real wf, ft, fc;
  weighfac(stress_params, wf);
  std::vector<Real> r(3);
  ft = f(_ft0, _at, intnl[0]);
  fc = f(_fc0, _ac, intnl[1]);
  flowPotential(stress_params, intnl, r);
  h[0] = wf * ft / _gt[_qp] * r[2];
  h[1] = -(1. - wf) * fc / _gc[_qp] * r[0];
}

void
DamagePlasticityStressUpdate::dhardPotential_dstress(const std::vector<Real> & stress_params,
                                                     const std::vector<Real> & intnl,
                                                     std::vector<std::vector<Real>> & dh_dsig) const
{
  Real wf, ft, fc;
  std::vector<Real> dwf(3);
  dweighfac(stress_params, wf, dwf);
  ft = f(_ft0, _at, intnl[0]);
  fc = f(_fc0, _ac, intnl[1]);

  std::vector<Real> r(3);
  flowPotential(stress_params, intnl, r);
  std::vector<std::vector<Real>> dr_dsig(3, std::vector<Real>(3));
  dflowPotential_dstress(stress_params, intnl, dr_dsig);

  for (unsigned i = 0; i < _num_sp; ++i)
  {
    dh_dsig[0][i] = (wf * dr_dsig[2][i] + dwf[i] * r[2]) * ft / _gt[_qp];
    dh_dsig[1][i] = -((1. - wf) * dr_dsig[0][i] - dwf[i] * r[0]) * fc / _gc[_qp];
  }
}

void
DamagePlasticityStressUpdate::dhardPotential_dintnl(
    const std::vector<Real> & stress_params,
    const std::vector<Real> & intnl,
    std::vector<std::vector<Real>> & dh_dintnl) const
{
  Real wf, dft, dfc;
  weighfac(stress_params, wf);

  dft = df_dkappa(_ft0, _at, intnl[0]);
  dfc = df_dkappa(_fc0, _ac, intnl[1]);

  std::vector<Real> r(3);
  flowPotential(stress_params, intnl, r);

  dh_dintnl[0][0] = wf * dft / _gt[_qp] * r[2];
  dh_dintnl[0][1] = 0.;
  dh_dintnl[1][0] = 0.;
  dh_dintnl[1][1] = -(1 - wf) * dfc / _gc[_qp] * r[0];
}

void
DamagePlasticityStressUpdate::initialiseVarsV(const std::vector<Real> & trial_stress_params,
                                              const std::vector<Real> & intnl_old,
                                              std::vector<Real> & stress_params,
                                              Real & /* gaE */,
                                              std::vector<Real> & intnl) const
{
  setIntnlValuesV(trial_stress_params, stress_params, intnl_old, intnl);
}

void
DamagePlasticityStressUpdate::setIntnlValuesV(const std::vector<Real> & trial_stress_params,
                                              const std::vector<Real> & current_stress_params,
                                              const std::vector<Real> & intnl_old,
                                              std::vector<Real> & intnl) const
{
  RankTwoTensor sigma_trial = RankTwoTensor(
      trial_stress_params[0], trial_stress_params[1], trial_stress_params[2], 0, 0, 0);
  Real I1_trial = trial_stress_params[0] + trial_stress_params[1] + trial_stress_params[2];
  RankTwoTensor Identity_tensor = RankTwoTensor(1, 1, 1, 0, 0, 0);
  RankTwoTensor sigmadev_trial = sigma_trial - (I1_trial / 3.) * Identity_tensor;
  Real norm_sigmadev_trial = sigmadev_trial.L2norm();

  Real G = 0.5 * (_Eij[0][0] - _Eij[0][1]); // Lame's mu
  Real K = _Eij[0][1] + 2. * G / 3.;        // Bulk modulus
  Real C3 = 3. * K * _alfa_p;

  RankTwoTensor denominator_tensor =
      (2 * G / norm_sigmadev_trial) * sigmadev_trial + C3 * Identity_tensor;


  RankTwoTensor dsig = RankTwoTensor(trial_stress_params[0] - current_stress_params[0],
                                     trial_stress_params[1] - current_stress_params[1],
                                     trial_stress_params[2] - current_stress_params[2],
                                     0.,
                                     0.,
                                     0.);

// Implementing Eqn. (21) of Lee & Fenves, 2001, with backsubstituting eqn. (22) 
  Real lam = dsig.L2norm() / denominator_tensor.L2norm(); 

  std::vector<Real> h(2);
  hardPotential(current_stress_params, intnl_old, h);

  intnl[0] = intnl_old[0] + lam * h[0];
  intnl[1] = intnl_old[1] + lam * h[1];
}

void
DamagePlasticityStressUpdate::setIntnlDerivativesV(const std::vector<Real> & trial_stress_params,
                                                   const std::vector<Real> & current_stress_params,
                                                   const std::vector<Real> & intnl,
                                                   std::vector<std::vector<Real>> & dintnl) const
{
  RankTwoTensor sigma_trial = RankTwoTensor(
      trial_stress_params[0], trial_stress_params[1], trial_stress_params[2], 0, 0, 0);
  Real I1_trial = trial_stress_params[0] + trial_stress_params[1] + trial_stress_params[2];
  RankTwoTensor Identity_tensor = RankTwoTensor(1, 1, 1, 0, 0, 0);
  RankTwoTensor sigmadev_trial = sigma_trial - (I1_trial / 3.) * Identity_tensor;
  Real norm_sigmadev_trial = sigmadev_trial.L2norm();

  Real G = 0.5 * (_Eij[0][0] - _Eij[0][1]); // Lame's mu
  Real K = _Eij[0][1] + 2. * G / 3.;        // Bulk modulus
  Real C3 = 3. * K * _alfa_p;

  RankTwoTensor denominator_tensor =
      2 * G  * (sigmadev_trial / norm_sigmadev_trial) + C3 * Identity_tensor;
  RankTwoTensor dsig = RankTwoTensor(trial_stress_params[0] - current_stress_params[0],
                                     trial_stress_params[1] - current_stress_params[1],
                                     trial_stress_params[2] - current_stress_params[2],
                                     0.,
                                     0.,
                                     0.);

  Real lam = dsig.L2norm() / denominator_tensor.L2norm();

  std::vector<Real> dlam_dsig(3);
  for (unsigned i = 0; i < _num_sp; ++i)
    dlam_dsig[i] = dsig.L2norm() == 0. ? 0.
                                       : -(trial_stress_params[i] - current_stress_params[i]) /
                                             (dsig.L2norm() * denominator_tensor.L2norm());

  std::vector<Real> h(2);
  hardPotential(current_stress_params, intnl, h);
  std::vector<std::vector<Real>> dh_dsig(2, std::vector<Real>(3));
  dhardPotential_dstress(current_stress_params, intnl, dh_dsig);
  std::vector<std::vector<Real>> dh_dintnl(2, std::vector<Real>(2));
  dhardPotential_dintnl(current_stress_params, intnl, dh_dintnl);

  for (unsigned i = 0; i < _num_intnl; ++i)
    for (unsigned j = 0; j < _num_sp; ++j)
      dintnl[i][j] = dlam_dsig[j] * h[i] + lam * dh_dsig[i][j];
}

Real
DamagePlasticityStressUpdate::fbar(const Real & f0,
                                   const Real & a,
                                   const Real & exponent,
                                   const Real & kappa) const
{
  Real phi = 1. + a * (2. + a) * kappa;
  Real sqrt_phi = std::sqrt(phi);
  Real v = sqrt_phi;
  Real u = (1 + a) / a - sqrt_phi / a;
  Real cal_fbar = f0 * std::pow(u, exponent) * v;
  return (u > 0) ? cal_fbar : 1.E-6;
}

Real
DamagePlasticityStressUpdate::dfbar_dkappa(const Real & f0,
                                           const Real & a,
                                           const Real & exponent,
                                           const Real & kappa) const
{
  Real phi = 1. + a * (2. + a) * kappa;
  Real dphi_dkappa = a * (2. + a);
  Real sqrt_phi = std::sqrt(phi);
  Real v = sqrt_phi;
  Real u = (1 + a) / a - sqrt_phi / a;
  Real dv_dphi = 1. / (2 * v);
  Real du_dphi = -(1 / (2 * a)) * (1 / sqrt_phi);
  Real cal_dfbar_dkappa =
      f0 * (std::pow(u, exponent) * dv_dphi + exponent * std::pow(u, exponent - 1) * v * du_dphi) *
      dphi_dkappa;
  return (u > 0) ? cal_dfbar_dkappa : 0.;
}

Real
DamagePlasticityStressUpdate::f(const Real & f0, const Real & a, const Real & kappa) const
{
  Real phi = 1. + a * (2. + a) * kappa;
  Real sqrt_phi = std::sqrt(phi);
  Real v = phi;
  Real u = (1 + a) * sqrt_phi;
  Real cal_f = (f0 / a) * (u - v);
  return (u > v) ? cal_f : 1.E-6;
}

Real
DamagePlasticityStressUpdate::df_dkappa(const Real & f0, const Real & a, const Real & kappa) const
{
  Real phi = 1. + a * (2. + a) * kappa;
  Real dphi_dkappa = a * (2. + a);
  Real sqrt_phi = std::sqrt(phi);
  Real v = phi;
  Real u = (1 + a) * sqrt_phi;
  Real dv_dphi = 1.;
  Real du_dphi = (1 + a) / (2 * sqrt_phi);
  Real cal_df_dkappa = (f0 / a) * (du_dphi - dv_dphi) * dphi_dkappa;
  return (u > v) ? cal_df_dkappa : 0.;
}

Real
DamagePlasticityStressUpdate::beta(const std::vector<Real> & intnl) const
{
  Real fcbar, ftbar;
  fcbar = fbar(_fc0, _ac, 1. - _dc_bc, intnl[1]);
  ftbar = fbar(_ft0, _at, 1. - _dt_bt, intnl[0]);

  return (1. - _alfa) * (fcbar / ftbar) - (1. + _alfa);
}

Real
DamagePlasticityStressUpdate::dbeta0(const std::vector<Real> & intnl) const
{
  Real fcbar, ftbar, dftbar;
  fcbar = fbar(_fc0, _ac, 1. - _dc_bc, intnl[1]);
  ftbar = fbar(_ft0, _at, 1. - _dt_bt, intnl[0]);
  dftbar = dfbar_dkappa(_ft0, _at, 1. - _dt_bt, intnl[0]);
  return -(1. - _alfa) * fcbar * dftbar / Utility::pow<2>(ftbar);
}

Real
DamagePlasticityStressUpdate::dbeta1(const std::vector<Real> & intnl) const
{
  Real fcbar, ftbar, dfcbar;
  fcbar = fbar(_fc0, _ac, 1. - _dc_bc, intnl[1]);
  ftbar = fbar(_ft0, _at, 1. - _dt_bt, intnl[0]);
  dfcbar = dfbar_dkappa(_fc0, _ac, 1. - _dc_bc, intnl[1]);
  return dfcbar / ftbar * (1. - _alfa);
}

void
DamagePlasticityStressUpdate::weighfac(const std::vector<Real> & stress_params, Real & wf) const
{
  Real Dr = 0.;
  Real Nr = 0.;
  for (unsigned i = 0; i < _num_sp; ++i)
  {
    if (stress_params[i] > 0.)
    {
      Nr += stress_params[i];
      Dr += stress_params[i];
    }
    else
      Dr += -stress_params[i];
  }
  wf = Nr / Dr;
}

void
DamagePlasticityStressUpdate::dweighfac(const std::vector<Real> & stress_params,
                                        Real & wf,
                                        std::vector<Real> & dwf) const
{
  std::vector<Real> dNr(3, 0.), dDr(3, 0.);
  Real Dr = 0.;
  Real Nr = 0.;
  for (unsigned i = 0; i < _num_sp; ++i)
  {
    if (stress_params[i] > 0.)
    {
      Nr += stress_params[i];
      dNr[i] = 1.;
      Dr += stress_params[i];
      dDr[i] = 1.;
    }
    else
    {
      Dr += -stress_params[i];
      dDr[i] = -1.;
    }
  }
  wf = Nr / Dr;

  for (unsigned i = 0; i < _num_sp; ++i)
    dwf[i] = (dNr[i] - wf * dDr[i]) / Dr;
}

Real
DamagePlasticityStressUpdate::damageVar(const std::vector<Real> & stress_params,
                                        const std::vector<Real> & intnl) const
{
  Real sqrtPhi_t = std::sqrt(1. + _at * (2. + _at) * intnl[0]);
  if (_zt > sqrtPhi_t / _at)
    _tD[_qp] = 1. - std::pow(_zt - sqrtPhi_t / _at, _dt_bt);
  else
    _tD[_qp] = 1. - 1.E-6;

  Real wf;
  weighfac(stress_params, wf);
  Real s = _s0 + (1. - _s0) * wf;

  Real sqrtPhi_c;
  if (intnl[1] < 1.0)
    sqrtPhi_c = std::sqrt(1. + _ac * (2. + _ac) * intnl[1]);
  else
    sqrtPhi_c = std::sqrt(1. + _ac * (2. + _ac) * 0.99);

  _cD[_qp] = 1. - std::pow((_zc - sqrtPhi_c / _ac), _dc_bc);
  return 1. - (1. - s * _tD[_qp]) * (1. - _cD[_qp]);
}

void
DamagePlasticityStressUpdate::consistentTangentOperatorV(
    const RankTwoTensor & /* stress_trial */,
    const std::vector<Real> & /* trial_stress_params */,
    const RankTwoTensor & /*stress*/,
    const std::vector<Real> & /* stress_params */,
    Real /*gaE*/,
    const yieldAndFlow & /*smoothed_q*/,
    const RankFourTensor & elasticity_tensor,
    bool /* compute_full_tangent_operator */,
    const std::vector<std::vector<Real>> & /* dvar_dtrial */,
    RankFourTensor & cto)
{
  cto = elasticity_tensor;
  return;
}
