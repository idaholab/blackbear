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
 * approach, no separate scalar variable is needed. The inter-component coupling
 * (including monomer/cluster coupling) is captured via intra-variable Jacobian
 * entries using setJacobian, which requires the nodal_array_jacobians framework
 * branch.
 */
template <bool is_ad>
class GenericClusterDynamicsNodalKernelTempl : public GenericArrayNodalKernel<is_ad>
{
public:
  static InputParameters validParams();
  GenericClusterDynamicsNodalKernelTempl(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(GenericRealEigenVector<is_ad> & residual) override;
  virtual void computeQpJacobian() override;

  /// Absorption rate coefficient: beta(n) = beta0 * n^(1/3)
  Real beta(unsigned int n) const;

  /// Emission rate coefficient: alpha(n) = alpha0 * n^(1/3)
  Real alpha(unsigned int n) const;

  /// Monomer generation rate G_1
  const Real _generation;

  /// Linear sink coefficient k_s
  const Real _sink;

  /// Base absorption coefficient beta_0
  const Real _beta0;

  /// Base emission coefficient alpha_0
  const Real _alpha0;

  usingGenericArrayNodalKernelMembers;
};

typedef GenericClusterDynamicsNodalKernelTempl<false> ClusterDynamicsNodalKernel;
typedef GenericClusterDynamicsNodalKernelTempl<true> ADClusterDynamicsNodalKernel;
