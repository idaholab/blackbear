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

#include "ClusterGroupingLayout.h"
#include "GenericArrayNodalKernel.h"
#include "MooseEnum.h"

#include <vector>

/**
 * Grouped L0/L1 cluster-dynamics nodal kernel.
 *
 * This object preserves the monomer and a user-selected explicit size range,
 * then stores the large-size tail in grouped bins carrying the first two
 * moments of the in-bin size distribution via the linear coefficients L0 and
 * L1. The grouped-tail dynamics follow the same practical moment formulation
 * used in the standalone Cu grouped-moments solver.
 */
template <bool is_ad>
class GenericClusterDynamicsGroupedNodalKernelTempl : public GenericArrayNodalKernel<is_ad>
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
  GenericClusterDynamicsGroupedNodalKernelTempl(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(GenericRealEigenVector<is_ad> & residual) override;
  virtual RealEigenVector computeQpJacobian() override;

  Real beta(unsigned int n) const;
  Real alpha(unsigned int n) const;
  Real atomicVolume() const;
  Real monomerDiffusivity() const;
  Real radius(unsigned int n) const;
  Real bindingEnergy(unsigned int n) const;
  void ensureCoefficientCache() const;

  template <typename Scalar>
  Scalar concentrationAt(const std::vector<Scalar> & state, unsigned int n) const;

  template <typename Scalar>
  void fillConcentrationCache(const std::vector<Scalar> & state,
                              std::vector<Scalar> & concentration) const;

  template <typename Scalar>
  void fillFluxCache(const std::vector<Scalar> & concentration, std::vector<Scalar> & flux) const;

  template <typename Scalar>
  void computeGroupedResidualFromState(const std::vector<Scalar> & state,
                                       std::vector<Scalar> & residual_values) const;

  template <typename Scalar>
  Scalar computeGroupedBinResidualComponent(const std::vector<Scalar> & state,
                                            const ClusterGroupingBin & bin,
                                            unsigned int component) const;

  const Real _generation;
  const Real _sink;
  const RateModel _rate_model;
  const Real _beta0;
  const Real _alpha0;
  const Real _temperature;
  const Real _monomer_diffusivity;
  const DiffusivityModel _diffusivity_model;
  const BindingEnergyModel _binding_energy_model;
  const Real _D0;
  const Real _Q;
  const Real _radiation_enhanced_factor;
  const Real _sigma;
  const Real _atomic_volume;
  const Real _Omega;
  const Real _DeltaS;
  const Real _binding_energy_constant;
  const Real _binding_energy_enthalpy;
  const Real _binding_energy_entropy;
  const Real _binding_energy_coefficient;
  const Real _binding_energy_exponent;
  const std::vector<Real> _binding_energy_table;
  const std::vector<Real> _formation_energy_table;

  const unsigned int _num_cluster_sizes;
  const bool _use_grouping;
  const unsigned int _explicit_max;
  const ClusterGroupingSpacing _group_spacing;
  const unsigned int _group_linear_width;
  const Real _group_geometric_ratio;
  const bool _enforce_group_nonnegative;
  const Real _group_nonnegative_tolerance_factor;

  const ClusterGroupingLayout _layout;

  mutable std::vector<Real> _beta_cache;
  mutable std::vector<Real> _alpha_cache;
  mutable unsigned int _cache_size = 0;

  usingGenericArrayNodalKernelMembers;
};

typedef GenericClusterDynamicsGroupedNodalKernelTempl<false> ClusterDynamicsGroupedNodalKernel;
typedef GenericClusterDynamicsGroupedNodalKernelTempl<true> ADClusterDynamicsGroupedNodalKernel;
