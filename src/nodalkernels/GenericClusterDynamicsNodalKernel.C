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
#include "FEProblemBase.h"

registerMooseObject("BlackBearApp", ClusterDynamicsNodalKernel);
registerMooseObject("BlackBearApp", ADClusterDynamicsNodalKernel);

template <bool is_ad>
InputParameters
GenericClusterDynamicsNodalKernelTempl<is_ad>::validParams()
{
  InputParameters params = GenericArrayNodalKernel<is_ad>::validParams();
  params.addClassDescription(
      "Cluster dynamics nodal kernel for all cluster sizes (1 through N) "
      "in a single array variable. Array index i corresponds to cluster size n = i+1.");
  params.addRequiredParam<Real>("generation", "Monomer generation rate G_1");
  params.addRequiredParam<Real>("sink", "Linear sink coefficient k_s for monomer loss");
  params.addRequiredParam<Real>("beta0", "Base absorption coefficient in beta_n = beta0*n^(1/3)");
  params.addRequiredParam<Real>("alpha0", "Base emission coefficient in alpha_n = alpha0*n^(1/3)");
  return params;
}

template <bool is_ad>
GenericClusterDynamicsNodalKernelTempl<is_ad>::GenericClusterDynamicsNodalKernelTempl(
    const InputParameters & parameters)
  : GenericArrayNodalKernel<is_ad>(parameters),
    _generation(this->template getParam<Real>("generation")),
    _sink(this->template getParam<Real>("sink")),
    _beta0(this->template getParam<Real>("beta0")),
    _alpha0(this->template getParam<Real>("alpha0"))
{
  if (!(this->_fe_problem.useHashTableMatrixAssembly()))
    mooseError("ClusterDynamicsNodalKernel requires Problem/use_hash_table_matrix_assembly = true");
}

template <bool is_ad>
Real
GenericClusterDynamicsNodalKernelTempl<is_ad>::beta(const unsigned int n) const
{
  return _beta0 * std::cbrt(static_cast<Real>(n));
}

template <bool is_ad>
Real
GenericClusterDynamicsNodalKernelTempl<is_ad>::alpha(const unsigned int n) const
{
  return _alpha0 * std::cbrt(static_cast<Real>(n));
}

template <bool is_ad>
void
GenericClusterDynamicsNodalKernelTempl<is_ad>::computeQpResidual(
    GenericRealEigenVector<is_ad> & residual)
{
  const auto n_comp = _u[_qp].size();
  residual.resize(n_comp);

  // c(i) = concentration of cluster of size n = i+1
  const auto c1 = _u[_qp](0); // monomer concentration

  // Component 0: monomer rate equation
  // dC_1/dt = G_1 - k_s*C_1
  //         - 2*beta(1)*C_1^2                        [monomer+monomer -> dimer]
  //         - sum_{i=1}^{N-1} beta(i+1)*C_1*C_{i+1} [monomer absorbed by larger cluster]
  //         + 2*alpha(2)*C_2                          [dimer dissociation]
  //         + sum_{i=2}^{N-1} alpha(i+1)*C_{i+1}     [larger cluster emission]
  {
    auto absorption = 2.0 * beta(1) * c1 * c1;
    GenericReal<is_ad> emission = 0.0;
    for (auto j = 1; j < n_comp; ++j)
    {
      const unsigned int nj = j + 1; // cluster size at index j
      absorption += beta(nj) * c1 * _u[_qp](j);
      emission += (j == 1 ? 2.0 : 1.0) * alpha(nj) * _u[_qp](j);
    }
    residual(0) = -(_generation - _sink * c1 - absorption + emission);
  }

  // Components i >= 1: cluster size n = i+1 >= 2
  // dC_n/dt = growth_in - growth_out + emit_in - emit_out
  // where:
  //   growth_in  = beta(n-1)*C_1*C_{n-1}  [cluster of size n-1 absorbs a monomer]
  //              = 0.5*beta(1)*C_1^2       [special case n=2: two monomers combine]
  //   growth_out = beta(n)*C_1*C_n         [cluster of size n absorbs a monomer]
  //   emit_in    = alpha(n+1)*C_{n+1}      [cluster of size n+1 emits a monomer]
  //   emit_out   = alpha(n)*C_n            [cluster of size n emits a monomer]
  for (auto i = 1; i < n_comp; ++i)
  {
    const unsigned int n = i + 1;
    const auto c_n = _u[_qp](i);
    const auto c_nm1 = _u[_qp](i - 1); // for i=1: c_nm1 = c(0) = monomer

    const auto growth_in = (n == 2) ? 0.5 * beta(1) * c1 * c1 : beta(n - 1) * c1 * c_nm1;
    const auto growth_out = beta(n) * c1 * c_n;
    const auto emit_in = (i + 1 < n_comp) ? alpha(n + 1) * _u[_qp](i + 1) : 0.0;
    const auto emit_out = alpha(n) * c_n;

    residual(i) = -(growth_in - growth_out + emit_in - emit_out);
  }
}

// ---------------------------------------------------------------------------
// Non-AD Jacobian: intra-variable coupling via setJacobian(row, col, value)
// row = residual component index (cluster size n = row+1)
// col = variable component being differentiated with respect to (size n = col+1)
// ---------------------------------------------------------------------------
template <>
void
GenericClusterDynamicsNodalKernelTempl<false>::computeQpJacobian()
{
  const auto n_comp = static_cast<unsigned int>(_u[_qp].size());
  const Real c1 = _u[_qp](0);

  // --- Row 0: monomer ---
  // F(0) = -(G_1 - k_s*c(0) - 2*beta(1)*c(0)^2
  //          - sum_{j>=1} beta(j+1)*c(0)*c(j)
  //          + 2*alpha(2)*c(1)
  //          + sum_{j>=2} alpha(j+1)*c(j))

  // d F(0)/d c(0) = k_s + 4*beta(1)*c(0) + sum_{j>=1} beta(j+1)*c(j)
  {
    Real d00 = _sink + 4.0 * beta(1) * c1;
    for (unsigned int j = 1; j < n_comp; ++j)
      d00 += beta(j + 1) * _u[_qp](j);
    setJacobian(0, 0, d00);
  }

  // d F(0)/d c(j) for j >= 1: beta(j+1)*c(0) - mu(j)*alpha(j+1)
  // where mu(j) = 2 for j=1 (dimer dissociation releases 2 monomers), 1 otherwise
  for (unsigned int j = 1; j < n_comp; ++j)
  {
    const Real mu = (j == 1) ? 2.0 : 1.0;
    setJacobian(0, j, beta(j + 1) * c1 - mu * alpha(j + 1));
  }

  // --- Rows i >= 1: cluster of size n = i+1 ---
  for (unsigned int i = 1; i < n_comp; ++i)
  {
    const unsigned int n = i + 1;

    // d F(i)/d c(0): coupling to monomer
    // n=2: F(1) = -(0.5*beta(1)*c(0)^2 - beta(2)*c(0)*c(1) + ...)
    //   -> d F(1)/d c(0) = -(beta(1)*c(0) - beta(2)*c(1))
    // n>2: F(i) = -(beta(n-1)*c(0)*c(i-1) - beta(n)*c(0)*c(i) + ...)
    //   -> d F(i)/d c(0) = -(beta(n-1)*c(i-1) - beta(n)*c(i))
    if (n == 2)
      setJacobian(i, 0, -(beta(1) * c1 - beta(n) * _u[_qp](i)));
    else
      setJacobian(i, 0, -(beta(n - 1) * _u[_qp](i - 1) - beta(n) * _u[_qp](i)));

    // d F(i)/d c(i-1): coupling to the next-smaller cluster (for n > 2 only;
    // for n=2 this is c(0) which is already handled above)
    if (i > 1)
      setJacobian(i, i - 1, -beta(n - 1) * c1);

    // d F(i)/d c(i): self (diagonal)
    // F(i) = -(... - beta(n)*c(0)*c(i) ... - alpha(n)*c(i))
    // -> d F(i)/d c(i) = beta(n)*c(0) + alpha(n)
    setJacobian(i, i, beta(n) * c1 + alpha(n));

    // d F(i)/d c(i+1): coupling to the next-larger cluster
    // F(i) = -(... + alpha(n+1)*c(i+1) ...)
    // -> d F(i)/d c(i+1) = -alpha(n+1)
    if (i + 1 < n_comp)
      setJacobian(i, i + 1, -alpha(n + 1));
  }
}

template <>
void
GenericClusterDynamicsNodalKernelTempl<true>::computeQpJacobian()
{
  mooseError("Internal error: computeQpJacobian should never be called for the AD version");
}

template class GenericClusterDynamicsNodalKernelTempl<false>;
template class GenericClusterDynamicsNodalKernelTempl<true>;
