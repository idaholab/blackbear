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

#pragma once

#include "GenericArrayNodalKernel.h"
#include "MooseEnum.h"

#include <vector>

/**
 * Combined cluster dynamics array nodal kernel for all cluster sizes (1 through N).
 *
 * Handles the monomer (size-1) and all larger clusters in a single array
 * variable. Array component index i corresponds to cluster size n = i+1:
 *   i=0  ->  n=1  (monomer)
 *   i=1  ->  n=2  (dimer)
 *   ...
 *   i=N-1 -> n=N
 *
 * Unlike the paired ClusterSizeOneNodalKernel + ClusterDynamicsArrayNodalKernel
 * approach, no separate scalar variable is needed.
 */
template <bool is_ad>
class GenericClusterDynamicsNodalKernelTempl : public GenericArrayNodalKernel<is_ad>
{
public:
  enum class RateModel
  {
    SIMPLE = 0,
    INTERFACIAL_ENERGY = 1
  };

  enum class DiffusivityModel
  {
    CONSTANT = 0,
    ARRHENIUS = 1
  };

  enum class BindingEnergyModel
  {
    INTERFACIAL_ENERGY = 0,
    CAPILLARY = 1,
    BINDING_ENERGY_TABLE = 2,
    FORMATION_ENERGY_TABLE = 3
  };

  static InputParameters validParams();
  GenericClusterDynamicsNodalKernelTempl(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(GenericRealEigenVector<is_ad> & residual) override;
  virtual RealEigenVector computeQpJacobian() override;

  /// Absorption rate coefficient for the selected rate model
  Real beta(unsigned int n) const;

  /// Emission rate coefficient for the selected rate model
  Real alpha(unsigned int n) const;

  /// Atomic volume used by the interfacial-energy model
  Real atomicVolume() const;

  /// Monomer diffusivity used by the interfacial-energy model
  Real monomerDiffusivity() const;

  /// Radius of a cluster of size n used by the interfacial-energy model
  Real radius(unsigned int n) const;

  /// Binding energy for a cluster of size n used by the interfacial-energy model
  Real bindingEnergy(unsigned int n) const;

  /// Build or refresh cached size-dependent coefficients for the current array size
  void ensureCoefficientCache(unsigned int n_comp) const;

  /// Monomer generation rate G_1
  const Real _generation;

  /// Linear sink coefficient k_s
  const Real _sink;

  /// Selected rate model
  const RateModel _rate_model;

  /// Base absorption coefficient beta_0 for the simple model
  const Real _beta0;

  /// Base emission coefficient alpha_0 for the simple model
  const Real _alpha0;

  /// Temperature T for the interfacial-energy model
  const Real _temperature;

  /// Monomer diffusion coefficient for the interfacial-energy model
  const Real _monomer_diffusivity;

  /// Selected diffusivity model for the interfacial-energy rate model
  const DiffusivityModel _diffusivity_model;

  /// Selected binding-energy model for detailed-balance emission
  const BindingEnergyModel _binding_energy_model;

  /// Diffusion prefactor D0 for the Arrhenius diffusivity model
  const Real _D0;

  /// Activation energy Q for the Arrhenius diffusivity model [J]
  const Real _Q;

  /// Radiation-enhanced multiplier applied to the monomer diffusivity
  const Real _radiation_enhanced_factor;

  /// Interfacial energy sigma for interfacial-energy and thermal-surface binding models
  const Real _sigma;

  /// Atomic volume V_at for the interfacial-energy model
  const Real _atomic_volume;

  /// Enthalpy term Omega for the interfacial-energy model
  const Real _Omega;

  /// Non-configurational entropy term DeltaS for the interfacial-energy model
  const Real _DeltaS;

  /// Generic capillary binding-energy constant [J]
  const Real _binding_energy_constant;

  /// Generic capillary enthalpy term [J]
  const Real _binding_energy_enthalpy;

  /// Generic capillary entropy term [J/K]
  const Real _binding_energy_entropy;

  /// Generic capillary binding-energy coefficient [J]
  const Real _binding_energy_coefficient;

  /// Generic capillary size exponent
  const Real _binding_energy_exponent;

  /// Direct binding energies indexed as n = index + 2 [J]
  const std::vector<Real> _binding_energy_table;

  /// Formation energies indexed as n = index + 1 [J]
  const std::vector<Real> _formation_energy_table;

  /// Cached beta coefficients indexed by cluster size n
  mutable std::vector<Real> _beta_cache;

  /// Cached alpha coefficients indexed by cluster size n
  mutable std::vector<Real> _alpha_cache;

  /// Number of array components represented by the current coefficient cache
  mutable unsigned int _cache_size = 0;

  usingGenericArrayNodalKernelMembers;
};

typedef GenericClusterDynamicsNodalKernelTempl<false> ClusterDynamicsNodalKernel;
typedef GenericClusterDynamicsNodalKernelTempl<true> ADClusterDynamicsNodalKernel;
