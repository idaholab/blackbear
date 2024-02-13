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

#include "NEMLStressBase.h"
#include "Conversion.h"

#include <limits>
#include <type_traits>

InputParameters
NEMLStressBase::validParams()
{
  InputParameters params = ComputeStressBase::validParams();
  params.addCoupledVar("temperature", 0.0, "Coupled temperature");
  params.addParam<Real>("target_increment",
                        "L2 norm of the inelastic strain increment to target by adjusting the "
                        "timestep");
  params.addParam<bool>("debug",
                        false,
                        "Print history and strain state at the current quadrature point when a "
                        "NEML stress update fails.");
  return params;
}

NEMLStressBase::NEMLStressBase(const InputParameters & parameters)
  : ComputeStressBase(parameters),
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
    _inelastic_strain(declareProperty<RankTwoTensor>(_base_name + "inelastic_strain")),
    _compute_dt(isParamValid("target_increment")),
    _target_increment(_compute_dt ? getParam<Real>("target_increment") : 0.0),
    _inelastic_strain_old(
        _compute_dt ? &getMaterialPropertyOld<RankTwoTensor>(_base_name + "inelastic_strain")
                    : nullptr),
    _material_dt(_compute_dt ? &declareProperty<Real>("material_timestep_limit") : nullptr),
    _damage_index(nullptr),
    _debug(getParam<bool>("debug"))
{
  // We're letting NEML write to raw pointers. Best make sure the stored types are
  // the same on both ends.
  static_assert(std::is_same<Real, double>::value,
                "MOOSE/libMesh must be compiled with double precision Real types");
}

void
NEMLStressBase::computeQpStress()
{
  // We must update:
  // 1) _stress
  // 2) _Jacobian_mult
  // 3) _elastic_strain
  // 4) _history

  // First do some declaration and translation
  double s_np1[6];
  double s_n[6];
  rankTwoTensorToNeml(_stress_old[_qp], s_n);

  double e_np1[6];
  rankTwoTensorToNeml(_mechanical_strain[_qp], e_np1);
  double e_n[6];
  rankTwoTensorToNeml(_mechanical_strain_old[_qp], e_n);

  double t_np1 = _t;
  double t_n = _t - _dt;

  double T_np1 = _temperature[_qp];
  double T_n = _temperature_old[_qp];

  mooseAssert(_model->nstore() == _hist[_qp].size(), "History data storage size mismatch");
  double * const h_np1 = (_model->nstore() > 0 ? _hist[_qp].data() : nullptr);
  mooseAssert(_model->nstore() == _hist_old[_qp].size(), "History data storage size mismatch");
  const double * const h_n = (_model->nstore() > 0 ? _hist_old[_qp].data() : nullptr);

  double A_np1[36];

  double u_np1;
  double u_n = _energy_old[_qp];

  double p_np1;
  double p_n = _dissipation_old[_qp];

  double estrain[6];

  // Actually call the update
  try
  {
    _model->update_sd(
        e_np1, e_n, T_np1, T_n, t_np1, t_n, s_np1, s_n, h_np1, h_n, A_np1, u_np1, u_n, p_np1, p_n);
  }
  catch (const neml::NEMLError & e)
  {
    if (_debug)
      mooseException("NEML stress update failed!\n",
                     "NEML message: ",
                     e.message(),
                     "\n",
                     "_qp=",
                     _qp,
                     " _q_point=",
                     _q_point[_qp],
                     " _current_elem->id()=",
                     _current_elem->id(),
                     '\n',

                     "Time increment: ",
                     _dt,
                     '\n',

                     "Old temperature: ",
                     _temperature_old[_qp],
                     '\n',
                     "New temperature: ",
                     _temperature[_qp],
                     '\n',

                     "Old echanical strain: ",
                     _mechanical_strain_old[_qp],
                     '\n',
                     "New mechanical strain: ",
                     _mechanical_strain[_qp],
                     '\n',

                     "Old stress: ",
                     _stress_old[_qp],
                     '\n',

                     "Old history: ",
                     Moose::stringify(_hist_old[_qp]),

                     "New history: ",
                     Moose::stringify(_hist[_qp]));
    else
      mooseException("NEML stress updated failed: ", e.message());
  }

  // Do more translation, now back to tensors
  nemlToRankTwoTensor(s_np1, _stress[_qp]);
  nemlToRankFourTensor(A_np1, _Jacobian_mult[_qp]);

  // Get the elastic strain
  try
  {
    _model->elastic_strains(s_np1, T_np1, h_np1, estrain);
  }
  catch (const neml::NEMLError & e)
  {
    mooseError("Error in NEML call for elastic strain: ", e.message());
  }

  // Translate
  nemlToRankTwoTensor(estrain, _elastic_strain[_qp]);

  // For EPP purposes calculate the inelastic strain
  double pstrain[6];
  for (unsigned int i = 0; i < 6; ++i)
    pstrain[i] = e_np1[i] - estrain[i];

  nemlToRankTwoTensor(pstrain, _inelastic_strain[_qp]);

  // compute material timestep
  if (_compute_dt)
  {
    const auto increment = (_inelastic_strain[_qp] - (*_inelastic_strain_old)[_qp]).L2norm();
    (*_material_dt)[_qp] =
        increment > 0 ? _dt * _target_increment / increment : std::numeric_limits<Real>::max();
  }

  // Store dissipation
  _energy[_qp] = u_np1;
  _dissipation[_qp] = p_np1;
  // get damage index
  if (_damage_index != nullptr)
    (*_damage_index)[_qp] = _model->get_damage(h_np1);
}

void
NEMLStressBase::initQpStatefulProperties()
{
  ComputeStressBase::initQpStatefulProperties();

  // Figure out initial history
  _hist[_qp].resize(_model->nstore());

  if (_model->nstore() > 0)
  {
    try
    {
      _model->init_store(_hist[_qp].data());
    }
    catch (const neml::NEMLError & e)
    {
      mooseError("Error initializing NEML history: ", e.message());
    }
  }
  _energy[_qp] = 0.0;
  _dissipation[_qp] = 0.0;

  if (_damage_index != nullptr)
    (*_damage_index)[_qp] = 0.0;
}

void
NEMLStressBase::rankTwoTensorToNeml(const RankTwoTensor & in, double * const out)
{
  double inds[6][2] = {{0, 0}, {1, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}};
  double mults[6] = {1.0, 1.0, 1.0, sqrt(2.0), sqrt(2.0), sqrt(2.0)};

  for (unsigned int i = 0; i < 6; ++i)
    out[i] = in(inds[i][0], inds[i][1]) * mults[i];
}

void
NEMLStressBase::nemlToRankTwoTensor(const double * const in, RankTwoTensor & out)
{
  static const unsigned int inds[6][2] = {{0, 0}, {1, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}};
  static const double mults[6] = {
      1.0, 1.0, 1.0, 1.0 / std::sqrt(2.0), 1.0 / std::sqrt(2.0), 1.0 / std::sqrt(2.0)};

  for (unsigned int i = 0; i < 6; ++i)
  {
    out(inds[i][0], inds[i][1]) = in[i] * mults[i];
    out(inds[i][1], inds[i][0]) = in[i] * mults[i];
  }
}

void
NEMLStressBase::nemlToRankFourTensor(const double * const in, RankFourTensor & out)
{
  static const unsigned int inds[6][2] = {{0, 0}, {1, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}};
  static const double mults[6] = {1.0, 1.0, 1.0, 1.0 / sqrt(2.0), 1.0 / sqrt(2.0), 1.0 / sqrt(2.0)};

  for (unsigned int i = 0; i < 6; ++i)
    for (unsigned int j = 0; j < 6; ++j)
    {
      out(inds[i][0], inds[i][1], inds[j][0], inds[j][1]) = in[i * 6 + j] * (mults[i] * mults[j]);
      out(inds[i][1], inds[i][0], inds[j][0], inds[j][1]) = in[i * 6 + j] * (mults[i] * mults[j]);
      out(inds[i][0], inds[i][1], inds[j][1], inds[j][0]) = in[i * 6 + j] * (mults[i] * mults[j]);
      out(inds[i][1], inds[i][0], inds[j][1], inds[j][0]) = in[i * 6 + j] * (mults[i] * mults[j]);
    }
}

#endif // NEML_ENABLED
