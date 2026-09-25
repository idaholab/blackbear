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

#include "GenericClusterDynamicsNodalKernel.h"
#include "libmesh/libmesh_common.h"

#include <cstddef>

namespace
{
constexpr Real kB = 1.380649e-23;
constexpr Real eV = 1.602176634e-19;

std::vector<Real>
convertEvVectorToJ(const std::vector<Real> & values)
{
  std::vector<Real> converted(values.size());
  for (std::size_t i = 0; i < values.size(); ++i)
    converted[i] = values[i] * eV;
  return converted;
}
} // namespace

registerMooseObject("BlackBearApp", ClusterDynamicsNodalKernel);
registerMooseObject("BlackBearApp", ADClusterDynamicsNodalKernel);

template <bool is_ad>
InputParameters
GenericClusterDynamicsNodalKernelTempl<is_ad>::validParams()
{
  InputParameters params = GenericArrayNodalKernel<is_ad>::validParams();
  MooseEnum rate_model("simple interfacial_energy", "simple");
  MooseEnum diffusivity_model("constant arrhenius", "constant");
  MooseEnum binding_energy_model("interfacial_energy capillary binding_energy_table "
                                 "formation_energy_table",
                                 "interfacial_energy");
  params.addClassDescription(
      "Cluster dynamics nodal kernel for all cluster sizes (1 through N) in a single "
      "array variable. Array index i corresponds to cluster size n = i+1.");
  params.addRequiredParam<Real>("generation", "Monomer generation rate G_1");
  params.addRequiredParam<Real>("sink", "Linear sink coefficient k_s for monomer loss");
  params.addParam<MooseEnum>(
      "rate_model",
      rate_model,
      "Rate-coefficient model. 'simple' uses beta_n = beta0*n^(1/3) and "
      "alpha_n = alpha0*n^(1/3); 'interfacial_energy' derives coefficients from "
      "cluster geometry, interfacial energy, and detailed balance.");
  params.addParam<MooseEnum>(
      "diffusivity_model",
      diffusivity_model,
      "Diffusivity model used only when rate_model = interfacial_energy. "
      "'constant' uses monomer_diffusivity directly, while 'arrhenius' computes "
      "the diffusivity from D0, Q_eV, and temperature.");
  params.addParam<MooseEnum>(
      "binding_energy_model",
      binding_energy_model,
      "Binding-energy model used by detailed-balance emission when rate_model = "
      "interfacial_energy. 'interfacial_energy' uses Omega, DeltaS, sigma, and atomic "
      "volume; 'capillary' uses A - B*(n^p - (n-1)^p), where A may be fixed or "
      "temperature dependent and B may be supplied directly or computed from sigma and "
      "atomic_volume; 'binding_energy_table' reads direct binding energies for n >= 2; "
      "and 'formation_energy_table' computes E_b(n) = E_f(n-1) + E_f(1) - E_f(n).");
  params.addParam<Real>("beta0", 0.0, "Base absorption coefficient for rate_model = simple.");
  params.addParam<Real>("alpha0", 0.0, "Base emission coefficient for rate_model = simple.");
  params.addParam<Real>(
      "temperature", 0.0, "Temperature T [K] for the interfacial-energy rate model.");
  params.addParam<Real>("monomer_diffusivity",
                        0.0,
                        "Monomer diffusion coefficient [m^2/s] for diffusivity_model = constant.");
  params.addParam<Real>(
      "D0", 0.0, "Diffusion prefactor D0 [m^2/s] for diffusivity_model = arrhenius.");
  params.addParam<Real>("Q_eV", 0.0, "Activation energy Q [eV] for diffusivity_model = arrhenius.");
  params.addParam<Real>("radiation_enhanced_factor",
                        1.0,
                        "Multiplicative radiation-enhanced factor applied to the monomer "
                        "diffusivity for the interfacial-energy model.");
  params.addParam<Real>(
      "sigma",
      0.0,
      "Interfacial energy sigma [J/m^2] for binding_energy_model = interfacial_energy "
      "or capillary when the capillary surface coefficient is computed from sigma.");
  params.addParam<Real>(
      "atomic_volume", 0.0, "Atomic volume V_at [m^3] for the interfacial-energy model.");
  params.addParam<Real>(
      "Omega_kB_K", 0.0, "Enthalpy term Omega [k_B*K] for the interfacial-energy model.");
  params.addParam<Real>(
      "DeltaS_kB",
      0.0,
      "Non-configurational entropy term DeltaS [k_B] for the interfacial-energy model.");
  params.addParam<Real>("binding_energy_constant_eV",
                        0.0,
                        "Constant A [eV] in binding_energy_model = capillary: "
                        "E_b(n) = A - B*(n^p - (n-1)^p).");
  params.addParam<Real>(
      "binding_energy_enthalpy_kB_K",
      0.0,
      "Enthalpy-like term H [k_B*K] for the temperature-dependent constant in "
      "binding_energy_model = capillary: E_b(n) = (H - T*S)*k_B - B*(n^p - (n-1)^p).");
  params.addParam<Real>("binding_energy_entropy_kB",
                        0.0,
                        "Entropy-like term S [k_B] for the temperature-dependent constant in "
                        "binding_energy_model = capillary.");
  params.addParam<Real>(
      "binding_energy_coefficient_eV",
      0.0,
      "Direct coefficient B [eV] in binding_energy_model = capillary. Leave this at zero "
      "and provide sigma to compute B from interfacial energy instead.");
  params.addParam<Real>(
      "binding_energy_exponent", 2.0 / 3.0, "Exponent p in binding_energy_model = capillary.");
  params.addParam<std::vector<Real>>(
      "binding_energy_table_eV",
      std::vector<Real>(),
      "Direct binding energies [eV] for binding_energy_model = binding_energy_table. "
      "Entry 0 corresponds to cluster size n = 2.");
  params.addParam<std::vector<Real>>(
      "formation_energy_table_eV",
      std::vector<Real>(),
      "Formation energies [eV] for binding_energy_model = formation_energy_table. "
      "Entry 0 corresponds to cluster size n = 1.");
  return params;
}

template <bool is_ad>
GenericClusterDynamicsNodalKernelTempl<is_ad>::GenericClusterDynamicsNodalKernelTempl(
    const InputParameters & parameters)
  : GenericArrayNodalKernel<is_ad>(parameters),
    _generation(this->template getParam<Real>("generation")),
    _sink(this->template getParam<Real>("sink")),
    _rate_model(this->template getParam<MooseEnum>("rate_model").template getEnum<RateModel>()),
    _beta0(this->template getParam<Real>("beta0")),
    _alpha0(this->template getParam<Real>("alpha0")),
    _temperature(this->template getParam<Real>("temperature")),
    _monomer_diffusivity(this->template getParam<Real>("monomer_diffusivity")),
    _diffusivity_model(this->template getParam<MooseEnum>("diffusivity_model")
                           .template getEnum<DiffusivityModel>()),
    _binding_energy_model(this->template getParam<MooseEnum>("binding_energy_model")
                              .template getEnum<BindingEnergyModel>()),
    _D0(this->template getParam<Real>("D0")),
    _Q(this->template getParam<Real>("Q_eV") * eV),
    _radiation_enhanced_factor(this->template getParam<Real>("radiation_enhanced_factor")),
    _sigma(this->template getParam<Real>("sigma")),
    _atomic_volume(this->template getParam<Real>("atomic_volume")),
    _Omega(this->template getParam<Real>("Omega_kB_K") * kB),
    _DeltaS(this->template getParam<Real>("DeltaS_kB") * kB),
    _binding_energy_constant(this->template getParam<Real>("binding_energy_constant_eV") * eV),
    _binding_energy_enthalpy(this->template getParam<Real>("binding_energy_enthalpy_kB_K") * kB),
    _binding_energy_entropy(this->template getParam<Real>("binding_energy_entropy_kB") * kB),
    _binding_energy_coefficient(this->template getParam<Real>("binding_energy_coefficient_eV") *
                                eV),
    _binding_energy_exponent(this->template getParam<Real>("binding_energy_exponent")),
    _binding_energy_table(
        convertEvVectorToJ(this->template getParam<std::vector<Real>>("binding_energy_table_eV"))),
    _formation_energy_table(
        convertEvVectorToJ(this->template getParam<std::vector<Real>>("formation_energy_table_eV")))
{
  if (_rate_model == RateModel::SIMPLE)
  {
    if (_beta0 <= 0.0)
      mooseError("ClusterDynamicsNodalKernel with rate_model = simple requires beta0 > 0.");
    if (_alpha0 <= 0.0)
      mooseError("ClusterDynamicsNodalKernel with rate_model = simple requires alpha0 > 0.");
  }
  else
  {
    if (_temperature <= 0.0)
      mooseError("ClusterDynamicsNodalKernel with rate_model = interfacial_energy requires "
                 "temperature > 0.");
    if (_diffusivity_model == DiffusivityModel::CONSTANT)
    {
      if (_monomer_diffusivity <= 0.0)
        mooseError("ClusterDynamicsNodalKernel with rate_model = interfacial_energy and "
                   "diffusivity_model = constant requires monomer_diffusivity > 0.");
    }
    else
    {
      if (_D0 <= 0.0)
        mooseError("ClusterDynamicsNodalKernel with rate_model = interfacial_energy and "
                   "diffusivity_model = arrhenius requires D0 > 0.");
      if (_Q <= 0.0)
        mooseError("ClusterDynamicsNodalKernel with rate_model = interfacial_energy and "
                   "diffusivity_model = arrhenius requires Q_eV > 0.");
    }
    if (_atomic_volume <= 0.0)
      mooseError("ClusterDynamicsNodalKernel with rate_model = interfacial_energy requires "
                 "atomic_volume > 0.");
    if (_radiation_enhanced_factor <= 0.0)
      mooseError("ClusterDynamicsNodalKernel with rate_model = interfacial_energy requires "
                 "radiation_enhanced_factor > 0.");
    if (_binding_energy_model == BindingEnergyModel::INTERFACIAL_ENERGY)
    {
      if (_sigma <= 0.0)
        mooseError("ClusterDynamicsNodalKernel with binding_energy_model = interfacial_energy "
                   "requires sigma > 0.");
      if (_Omega <= 0.0)
        mooseError("ClusterDynamicsNodalKernel with binding_energy_model = interfacial_energy "
                   "requires Omega_kB_K > 0.");
      if (_DeltaS <= 0.0)
        mooseError("ClusterDynamicsNodalKernel with binding_energy_model = interfacial_energy "
                   "requires DeltaS_kB > 0.");
    }
    else if (_binding_energy_model == BindingEnergyModel::CAPILLARY)
    {
      const bool has_fixed_constant = _binding_energy_constant != 0.0;
      const bool has_thermal_constant =
          _binding_energy_enthalpy != 0.0 || _binding_energy_entropy != 0.0;
      const bool has_direct_coefficient = _binding_energy_coefficient != 0.0;
      const bool has_surface_coefficient = _sigma != 0.0;

      if (has_fixed_constant && has_thermal_constant)
        mooseError("ClusterDynamicsNodalKernel with binding_energy_model = capillary cannot use "
                   "both binding_energy_constant_eV and binding_energy_enthalpy_kB_K/"
                   "binding_energy_entropy_kB.");
      if (has_direct_coefficient && has_surface_coefficient)
        mooseError("ClusterDynamicsNodalKernel with binding_energy_model = capillary cannot use "
                   "both binding_energy_coefficient_eV and sigma.");
      if (!has_fixed_constant && !has_thermal_constant && !has_direct_coefficient &&
          !has_surface_coefficient)
        mooseError("ClusterDynamicsNodalKernel with binding_energy_model = capillary requires "
                   "a constant term, a thermal H/S term, a direct coefficient, or sigma.");
      if (has_surface_coefficient && _sigma <= 0.0)
        mooseError("ClusterDynamicsNodalKernel with binding_energy_model = capillary requires "
                   "sigma > 0 when sigma is used for the size coefficient.");
      if (_binding_energy_exponent <= 0.0)
        mooseError("ClusterDynamicsNodalKernel requires binding_energy_exponent > 0.");
    }
    else if (_binding_energy_model == BindingEnergyModel::BINDING_ENERGY_TABLE)
    {
      if (_binding_energy_table.empty())
        mooseError("ClusterDynamicsNodalKernel with binding_energy_model = binding_energy_table "
                   "requires binding_energy_table_eV.");
    }
    else if (_binding_energy_model == BindingEnergyModel::FORMATION_ENERGY_TABLE)
    {
      if (_formation_energy_table.size() < 2)
        mooseError("ClusterDynamicsNodalKernel with binding_energy_model = formation_energy_table "
                   "requires at least E_f(1) and E_f(2).");
    }
  }
}

template <bool is_ad>
Real
GenericClusterDynamicsNodalKernelTempl<is_ad>::beta(const unsigned int n) const
{
  if (n < _beta_cache.size())
    return _beta_cache[n];

  if (_rate_model == RateModel::SIMPLE)
    return _beta0 * std::cbrt(static_cast<Real>(n));

  return 4.0 * libMesh::pi * (radius(1) + radius(n)) * monomerDiffusivity() / atomicVolume();
}

template <bool is_ad>
Real
GenericClusterDynamicsNodalKernelTempl<is_ad>::alpha(const unsigned int n) const
{
  if (n < _alpha_cache.size())
    return _alpha_cache[n];

  if (_rate_model == RateModel::SIMPLE)
    return _alpha0 * std::cbrt(static_cast<Real>(n));

  if (n <= 1)
    mooseError("alpha(n) is only defined for cluster sizes n >= 2 in interfacial_energy mode.");

  return beta(n - 1) * std::exp(-bindingEnergy(n) / (kB * _temperature));
}

template <bool is_ad>
Real
GenericClusterDynamicsNodalKernelTempl<is_ad>::atomicVolume() const
{
  return _atomic_volume;
}

template <bool is_ad>
Real
GenericClusterDynamicsNodalKernelTempl<is_ad>::monomerDiffusivity() const
{
  Real diffusivity = 0.0;
  if (_diffusivity_model == DiffusivityModel::CONSTANT)
    diffusivity = _monomer_diffusivity;
  else
    diffusivity = _D0 * std::exp(-_Q / (kB * _temperature));

  return _radiation_enhanced_factor * diffusivity;
}

template <bool is_ad>
Real
GenericClusterDynamicsNodalKernelTempl<is_ad>::radius(const unsigned int n) const
{
  return std::cbrt(3.0 * static_cast<Real>(n) * atomicVolume() / (4.0 * libMesh::pi));
}

template <bool is_ad>
Real
GenericClusterDynamicsNodalKernelTempl<is_ad>::bindingEnergy(const unsigned int n) const
{
  if (n <= 1)
    mooseError("bindingEnergy(n) is only defined for cluster sizes n >= 2.");

  if (_binding_energy_model == BindingEnergyModel::CAPILLARY)
  {
    const bool has_fixed_constant = _binding_energy_constant != 0.0;
    const bool has_direct_coefficient = _binding_energy_coefficient != 0.0;
    const Real constant_term =
        has_fixed_constant ? _binding_energy_constant
                           : _binding_energy_enthalpy - _temperature * _binding_energy_entropy;
    const Real size_coefficient =
        has_direct_coefficient
            ? _binding_energy_coefficient
            : std::cbrt(36.0 * libMesh::pi) * std::pow(atomicVolume(), 2.0 / 3.0) * _sigma;
    return constant_term -
           size_coefficient * (std::pow(static_cast<Real>(n), _binding_energy_exponent) -
                               std::pow(static_cast<Real>(n - 1), _binding_energy_exponent));
  }

  if (_binding_energy_model == BindingEnergyModel::BINDING_ENERGY_TABLE)
  {
    const auto index = n - 2;
    if (index >= _binding_energy_table.size())
      mooseError("binding_energy_table_eV does not contain a value for cluster size n = ", n, ".");
    return _binding_energy_table[index];
  }

  if (_binding_energy_model == BindingEnergyModel::FORMATION_ENERGY_TABLE)
  {
    if (n > _formation_energy_table.size())
      mooseError(
          "formation_energy_table_eV does not contain a value for cluster size n = ", n, ".");
    return _formation_energy_table[n - 2] + _formation_energy_table[0] -
           _formation_energy_table[n - 1];
  }

  const Real surface_prefactor =
      std::cbrt(36.0 * libMesh::pi) * std::pow(atomicVolume(), 2.0 / 3.0) * _sigma;
  return _Omega - _temperature * _DeltaS -
         surface_prefactor * (std::pow(static_cast<Real>(n), 2.0 / 3.0) -
                              std::pow(static_cast<Real>(n - 1), 2.0 / 3.0));
}

template <bool is_ad>
void
GenericClusterDynamicsNodalKernelTempl<is_ad>::ensureCoefficientCache(
    const unsigned int n_comp) const
{
  if (_cache_size == n_comp)
    return;

  _beta_cache.assign(n_comp + 1, 0.0);
  _alpha_cache.assign(n_comp + 1, 0.0);

  if (_rate_model == RateModel::SIMPLE)
  {
    for (unsigned int n = 1; n <= n_comp; ++n)
    {
      const Real n_to_one_third = std::cbrt(static_cast<Real>(n));
      _beta_cache[n] = _beta0 * n_to_one_third;
      if (n >= 2)
        _alpha_cache[n] = _alpha0 * n_to_one_third;
    }
  }
  else
  {
    const Real diffusivity = monomerDiffusivity();
    const Real atomic_volume = atomicVolume();
    const Real inv_atomic_volume = 1.0 / atomic_volume;
    const Real radius_prefactor = std::cbrt(3.0 * atomic_volume / (4.0 * libMesh::pi));
    const Real r1 = radius_prefactor;
    const Real thermal_factor = kB * _temperature;

    std::vector<Real> radius_cache(n_comp + 1, 0.0);
    for (unsigned int n = 1; n <= n_comp; ++n)
    {
      radius_cache[n] = radius_prefactor * std::cbrt(static_cast<Real>(n));
      _beta_cache[n] = 4.0 * libMesh::pi * (r1 + radius_cache[n]) * diffusivity * inv_atomic_volume;
    }

    for (unsigned int n = 2; n <= n_comp; ++n)
      _alpha_cache[n] = _beta_cache[n - 1] * std::exp(-bindingEnergy(n) / thermal_factor);
  }

  _cache_size = n_comp;
}

template <bool is_ad>
void
GenericClusterDynamicsNodalKernelTempl<is_ad>::computeQpResidual(
    GenericRealEigenVector<is_ad> & residual)
{
  const auto n_comp = _u[_qp].size();
  ensureCoefficientCache(n_comp);
  residual.resize(n_comp);

  // c(i) = concentration of cluster of size n = i+1
  const auto c1 = _u[_qp](0); // monomer concentration

  // Component 0: monomer rate equation
  // dC_1/dt = G_1 - k_s*C_1
  //         - 2*beta(1)*C_1^2                        [monomer+monomer -> dimer]
  //         - sum_{i=1}^{N-2} beta(i+1)*C_1*C_{i+1} [monomer absorbed by larger cluster]
  //         + 2*alpha(2)*C_2                          [dimer dissociation]
  //         + sum_{i=2}^{N-1} alpha(i+1)*C_{i+1}     [larger cluster emission]
  {
    auto absorption = n_comp > 1 ? 2.0 * beta(1) * c1 * c1 : 0.0;
    GenericReal<is_ad> emission = 0.0;
    for (auto j = 1; j < n_comp; ++j)
    {
      const unsigned int nj = j + 1; // cluster size at index j
      if (j + 1 < n_comp)
        absorption += beta(nj) * c1 * _u[_qp](j);
      emission += (j == 1 ? 2.0 : 1.0) * alpha(nj) * _u[_qp](j);
    }
    residual(0) = -(_generation - _sink * c1 - absorption + emission);
  }

  // Components i >= 1: cluster size n = i+1 >= 2
  // dC_n/dt = growth_in - growth_out + emit_in - emit_out
  // where:
  //   This form follows the single-species cluster flux definition and preserves mass:
  //   dC_n/dt = J_{n-1->n} - J_{n->n+1}, with
  //   J_{n->n+1} = beta(n)*C_1*C_n - alpha(n+1)*C_{n+1}. For n=2,
  //   J_{1->2} = beta(1)*C_1^2 - alpha(2)*C_2, so the absorption term is
  //   beta(1)*C_1^2 with no 1/2 factor.
  //   growth_in  = beta(n-1)*C_1*C_{n-1}  [cluster of size n-1 absorbs a monomer]
  //              = beta(1)*C_1^2          [special case n=2: monomer is also C_{n-1}]
  //   growth_out = beta(n)*C_1*C_n         [cluster of size n absorbs a monomer]
  //   emit_in    = alpha(n+1)*C_{n+1}      [cluster of size n+1 emits a monomer]
  //   emit_out   = alpha(n)*C_n            [cluster of size n emits a monomer]
  for (auto i = 1; i < n_comp; ++i)
  {
    const unsigned int n = i + 1;
    const auto c_n = _u[_qp](i);
    const auto c_nm1 = _u[_qp](i - 1); // for i=1: c_nm1 = c(0) = monomer

    const auto growth_in = beta(n - 1) * c1 * c_nm1;
    const auto growth_out = (i + 1 < n_comp) ? beta(n) * c1 * c_n : 0.0;
    const auto emit_in = (i + 1 < n_comp) ? alpha(n + 1) * _u[_qp](i + 1) : 0.0;
    const auto emit_out = alpha(n) * c_n;

    residual(i) = -(growth_in - growth_out + emit_in - emit_out);
  }
}

template <>
RealEigenVector
GenericClusterDynamicsNodalKernelTempl<false>::computeQpJacobian()
{
  const auto n_comp = static_cast<unsigned int>(_u[_qp].size());
  ensureCoefficientCache(n_comp);
  const Real c1 = _u[_qp](0);
  RealEigenVector jacobian = RealEigenVector::Zero(n_comp);

  // This MOOSE ArrayNodalKernel API supports only diagonal Jacobian entries for a
  // single array variable. We therefore provide the diagonal contribution here.
  {
    Real d00 = _sink + (n_comp > 1 ? 4.0 * beta(1) * c1 : 0.0);
    for (unsigned int j = 1; j + 1 < n_comp; ++j)
      d00 += beta(j + 1) * _u[_qp](j);
    jacobian(0) = d00;
  }

  for (unsigned int i = 1; i < n_comp; ++i)
  {
    const unsigned int n = i + 1;
    jacobian(i) = (i + 1 < n_comp ? beta(n) * c1 : 0.0) + alpha(n);
  }

  return jacobian;
}

template <>
RealEigenVector
GenericClusterDynamicsNodalKernelTempl<true>::computeQpJacobian()
{
  mooseError("Internal error: computeQpJacobian should never be called for the AD version");
}

template class GenericClusterDynamicsNodalKernelTempl<false>;
template class GenericClusterDynamicsNodalKernelTempl<true>;
