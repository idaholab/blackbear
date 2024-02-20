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

#include "CauchyStressFromNEML.h"

#include "NEMLStressBase.h"

registerMooseObject("BlackBearApp", CauchyStressFromNEML);

InputParameters
CauchyStressFromNEML::validParams()
{
  InputParameters params = ComputeLagrangianStressCauchy::validParams();

  params.addRequiredParam<FileName>("database", "Path to NEML XML database.");
  params.addRequiredParam<std::string>("model", "Model name in NEML database.");
  params.addCoupledVar("temperature", 0.0, "Coupled temperature");

  return params;
}

CauchyStressFromNEML::CauchyStressFromNEML(const InputParameters & parameters)
  : ComputeLagrangianStressCauchy(parameters),
    _fname(getParam<FileName>("database")),
    _mname(getParam<std::string>("model")),
    _temperature(coupledValue("temperature")),
    _temperature_old(coupledValueOld("temperature")),
    _history(declareProperty<std::vector<Real>>(_base_name + "history")),
    _history_old(getMaterialPropertyOld<std::vector<Real>>(_base_name + "history")),
    _energy(declareProperty<Real>(_base_name + "energy")),
    _energy_old(getMaterialPropertyOld<Real>(_base_name + "energy")),
    _dissipation(declareProperty<Real>(_base_name + "dissipation")),
    _dissipation_old(declareProperty<Real>(_base_name + "dissipation_old")),
    _linear_rotation(declareProperty<RankTwoTensor>(_base_name + "linear_rotation")),
    _linear_rotation_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "linear_rotation")),
    _cauchy_stress_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "cauchy_stress")),
    _mechanical_strain(getMaterialProperty<RankTwoTensor>(_base_name + "mechanical_strain")),
    _mechanical_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "mechanical_strain")),
    _inelastic_strain(declareProperty<RankTwoTensor>(_base_name + "inelastic_strain")),
    _elastic_strain(declareProperty<RankTwoTensor>(_base_name + "elastic_strain")),
    _dissipation_rate(declareProperty<Real>(_base_name + "dissipation_rate"))
{
  // Check that the file is readable
  MooseUtils::checkFileReadable(_fname);

  // Will throw an exception if it doesn't succeed
  try
  {
    _model = neml::parse_xml_unique(_fname, _mname);
  }
  catch (const neml::NEMLError & e)
  {
    paramError("Unable to load NEML model " + _mname + " from file " + _fname);
  }
}

void
CauchyStressFromNEML::reset_state(const std::vector<unsigned int> & indices, unsigned int qp)
{
  // Guard from zero, just for MOOSE...
  if (_model->nstore() == 0)
    return;

  // Form a NEML history object with the initial state
  std::vector<Real> init(_model->nstore(), 0);
  _model->init_store(&init.front());

  // Reset!
  for (auto i : indices)
    _history[qp][i] = init[i];
}

std::vector<unsigned int>
CauchyStressFromNEML::provide_indices(const std::vector<std::string> & to_reset)
{
  std::vector<unsigned int> indices;

  // Grab the state names...
  auto names = _model->report_internal_variable_names();

  // Iterate through names resetting each if found,
  // raise an error if you don't find it
  for (auto name : to_reset)
  {
    auto loc = std::find(names.begin(), names.end(), name);
    if (loc == names.end())
      mooseError("One of the state variables in the list "
                 "requested for resetting does not exist "
                 "in the NEML material model");
    unsigned int i = loc - names.begin();
    indices.push_back(i);
  }

  return indices;
}

void
CauchyStressFromNEML::computeQpCauchyStress()
{
  // Setup all the Mandel notation things we need
  double s_np1[6];
  double s_n[6];
  NEMLStressBase::rankTwoTensorToNeml(_cauchy_stress_old[_qp], s_n);

  // Strain
  double e_np1[6];
  NEMLStressBase::rankTwoTensorToNeml(_mechanical_strain[_qp], e_np1);
  double e_n[6];
  NEMLStressBase::rankTwoTensorToNeml(_mechanical_strain_old[_qp], e_n);

  // Vorticity
  RankTwoTensor L;
  if (_large_kinematics)
  {
    L = RankTwoTensor::Identity() - _inv_df[_qp];
  }
  else
  {
    L.zero();
  }
  _linear_rotation[_qp] = _linear_rotation_old[_qp] + (L - L.transpose()) / 2.0;

  double w_np1[3];
  rankTwoSkewToNEML(_linear_rotation[_qp], w_np1);
  double w_n[3];
  rankTwoSkewToNEML(_linear_rotation_old[_qp], w_n);

  // Time
  double t_np1 = _t;
  double t_n = _t - _dt;

  // Temperature
  double T_np1 = _temperature[_qp];
  double T_n = _temperature_old[_qp];

  // Internal state
  double * h_np1;
  const double * h_n;

  // Just to keep MOOSE debug happy
  if (_model->nstore() > 0)
  {
    h_np1 = &(_history[_qp][0]);
    h_n = &(_history_old[_qp][0]);
  }
  else
  {
    h_np1 = nullptr;
    h_n = nullptr;
  }

  // Energy
  double u_np1;
  double u_n = _energy_old[_qp];

  // Dissipation
  double p_np1;
  double p_n = _dissipation_old[_qp];

  // Tangent
  double A_np1[36];
  double B_np1[18];

  try
  {
    // Call NEML!
    if (_large_kinematics)
    {
      _model->update_ld_inc(e_np1,
                            e_n,
                            w_np1,
                            w_n,
                            T_np1,
                            T_n,
                            t_np1,
                            t_n,
                            s_np1,
                            s_n,
                            h_np1,
                            h_n,
                            A_np1,
                            B_np1,
                            u_np1,
                            u_n,
                            p_np1,
                            p_n);
    }
    else
    {
      _model->update_sd(e_np1,
                        e_n,
                        T_np1,
                        T_n,
                        t_np1,
                        t_n,
                        s_np1,
                        s_n,
                        h_np1,
                        h_n,
                        A_np1,
                        u_np1,
                        u_n,
                        p_np1,
                        p_n);
      std::fill(B_np1, B_np1 + 18, 0.0);
    }

    double estrain[6];
    _model->elastic_strains(s_np1, T_np1, h_np1, estrain);

    // Translate back from Mandel notation
    NEMLStressBase::nemlToRankTwoTensor(s_np1, _cauchy_stress[_qp]);
    recombineTangentNEML(A_np1, B_np1, _cauchy_jacobian[_qp]);
    _energy[_qp] = u_np1;
    _dissipation[_qp] = p_np1;
    _dissipation_rate[_qp] = (p_np1 - p_n) / (t_np1 - t_n);

    NEMLStressBase::nemlToRankTwoTensor(estrain, _elastic_strain[_qp]);
    _inelastic_strain[_qp] = _mechanical_strain[_qp] - _elastic_strain[_qp];
  }
  catch (const neml::NEMLError & e)
  {
    throw MooseException("NEML error: " + e.message());
  }
}

void
CauchyStressFromNEML::initQpStatefulProperties()
{
  ComputeLagrangianStressCauchy::initQpStatefulProperties();

  _history[_qp].resize(_model->nstore());
  try
  {
    // This is only needed because MOOSE whines about zero sized vectors
    // that are not initialized
    if (_history[_qp].size() > 0)
      _model->init_store(&_history[_qp].front());
  }
  catch (const neml::NEMLError & e)
  {
    throw MooseException("NEML error: " + e.message());
  }

  _linear_rotation[_qp].zero();

  _energy[_qp] = 0.0;
  _dissipation[_qp] = 0.0;
  _dissipation_rate[_qp] = 0.0;
}

void
rankTwoSkewToNEML(const RankTwoTensor & in, double * const out)
{
  out[0] = -in(1, 2);
  out[1] = in(0, 2);
  out[2] = -in(0, 1);
}

void
recombineTangentNEML(const double * const Dpart, const double * const Wpart, RankFourTensor & out)
{
  std::vector<double> data(81);
  neml::transform_fourth(Dpart, Wpart, &data[0]);
  out.fillFromInputVector(data, RankFourTensor::FillMethod::general);
}

#endif // NEML_ENABLED
