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

#ifdef NEML_ENABLED
#include "NEMLStress.h"

registerMooseObject("BlackBearApp", NEMLStress);

template <>
InputParameters
validParams<NEMLStress>()
{
  InputParameters params = validParams<ComputeStressBase>();
  params.addClassDescription("Use a constitutive model in the NEML library to compute stress");
  params.addRequiredParam<FileName>("database", "Path to NEML XML database.");
  params.addRequiredParam<std::string>("model", "Model name in NEML database.");
  params.addCoupledVar("temperature", 0.0, "Coupled temperature");
  return params;
}

NEMLStress::NEMLStress(const InputParameters & parameters)
  : ComputeStressBase(parameters),
    _fname(getParam<FileName>("database")),
    _mname(getParam<std::string>("model")),
    _hist(declareProperty<std::vector<Real>>(_base_name + "hist")),
    _hist_old(getMaterialPropertyOld<std::vector<Real>>(_base_name + "hist")),
    _mechanical_strain_old(
        getMaterialPropertyOldByName<RankTwoTensor>(_base_name + "mechanical_strain")),
    _stress_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "stress")),
    _energy(declareProperty<Real>(_base_name + "energy")),
    _energy_old(getMaterialPropertyOld<Real>(_base_name + "energy")),
    _dissipation(declareProperty<Real>(_base_name + "dissipation")),
    _dissipation_old(getMaterialPropertyOld<Real>(_base_name + "dissipation")),
    _temperature(coupledValue("temperature")),
    _temperature_old(coupledValueOld("temperature")),
    _inelastic_strain(declareProperty<RankTwoTensor>(_base_name + "inelastic_strain"))
{
  _model = neml::parse_xml_unique(_fname, _mname);
}

void
NEMLStress::computeQpStress()
{
  // We must update:
  // 1) _stress
  // 2) _Jacobian_mult
  // 3) _elastic_strain
  // 4) _history

  // First do some declaration and translation
  double s_np1[6];
  double s_n[6];
  RankTwoTensorToNeml(_stress_old[_qp], s_n);

  double e_np1[6];
  RankTwoTensorToNeml(_mechanical_strain[_qp], e_np1);
  double e_n[6];
  RankTwoTensorToNeml(_mechanical_strain_old[_qp], e_n);

  double t_np1 = _t;
  double t_n = _t - _dt;

  double T_np1 = _temperature[_qp];
  double T_n = _temperature_old[_qp];

  double * h_np1 = (_model->nhist() > 0 ? &(_hist[_qp][0]) : nullptr);
  const double * const h_n = (_model->nhist() > 0 ? &(_hist_old[_qp][0]) : nullptr);

  double A_np1[36];

  double u_np1;
  double u_n = _energy_old[_qp];

  double p_np1;
  double p_n = _dissipation_old[_qp];

  double estrain[6];

  int ier;

  // Actually call the update
  ier = _model->update_sd(
      e_np1, e_n, T_np1, T_n, t_np1, t_n, s_np1, s_n, h_np1, h_n, A_np1, u_np1, u_n, p_np1, p_n);

  if (ier != neml::SUCCESS)
    throw MooseException("NEML stress update failed!");

  // Do more translation, now back to tensors
  NemlToRankTwoTensor(s_np1, _stress[_qp]);
  NemlToRankFourTensor(A_np1, _Jacobian_mult[_qp]);

  // Get the elastic strain
  ier = _model->elastic_strains(s_np1, T_np1, h_np1, estrain);

  if (ier != neml::SUCCESS)
    mooseError("Error in NEML call for elastic strain!");

  // Translate
  NemlToRankTwoTensor(estrain, _elastic_strain[_qp]);

  // For EPP purposes calculate the inelastic strain
  double pstrain[6];
  for (unsigned int i = 0; i < 6; ++i)
    pstrain[i] = e_np1[i] - estrain[i];

  NemlToRankTwoTensor(pstrain, _inelastic_strain[_qp]);

  // Store dissipation
  _energy[_qp] = u_np1;
  _dissipation[_qp] = p_np1;
}

void
NEMLStress::initQpStatefulProperties()
{
  ComputeStressBase::initQpStatefulProperties();

  // Figure out initial history
  _hist[_qp].resize(_model->nhist());

  if (_model->nhist() > 0)
  {
    int ier = _model->init_hist(&(_hist[_qp][0]));
    if (ier != neml::SUCCESS)
      mooseError("Error initializing NEML history!");
  }

  _energy[_qp] = 0.0;
  _dissipation[_qp] = 0.0;
}

void
NEMLStress::RankTwoTensorToNeml(const RankTwoTensor & in, double * const out)
{
  double inds[6][2] = {{0, 0}, {1, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}};
  double mults[6] = {1.0, 1.0, 1.0, sqrt(2.0), sqrt(2.0), sqrt(2.0)};

  for (unsigned int i = 0; i < 6; ++i)
  {
    out[i] = in(inds[i][0], inds[i][1]) * mults[i];
  }
}

void
NEMLStress::NemlToRankTwoTensor(const double * const in, RankTwoTensor & out)
{
  double inds[6][2] = {{0, 0}, {1, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}};
  double mults[6] = {1.0, 1.0, 1.0, sqrt(2.0), sqrt(2.0), sqrt(2.0)};

  for (unsigned int i = 0; i < 6; ++i)
  {
    out(inds[i][0], inds[i][1]) = in[i] / mults[i];
    out(inds[i][1], inds[i][0]) = in[i] / mults[i];
  }
}

void
NEMLStress::NemlToRankFourTensor(const double * const in, RankFourTensor & out)
{
  double inds[6][2] = {{0, 0}, {1, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}};
  double mults[6] = {1.0, 1.0, 1.0, sqrt(2.0), sqrt(2.0), sqrt(2.0)};

  for (unsigned int i = 0; i < 6; ++i)
  {
    for (unsigned int j = 0; j < 6; ++j)
    {
      out(inds[i][0], inds[i][1], inds[j][0], inds[j][1]) = in[i * 6 + j] / (mults[i] * mults[j]);
      out(inds[i][1], inds[i][0], inds[j][0], inds[j][1]) = in[i * 6 + j] / (mults[i] * mults[j]);
      out(inds[i][0], inds[i][1], inds[j][1], inds[j][0]) = in[i * 6 + j] / (mults[i] * mults[j]);
      out(inds[i][1], inds[i][0], inds[j][1], inds[j][0]) = in[i * 6 + j] / (mults[i] * mults[j]);
    }
  }
}
#endif // NEML_ENABLED
