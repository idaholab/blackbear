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

#include "ConcreteExpansionEigenstrainBase.h"
#include "RankTwoTensor.h"

template <>
InputParameters
validParams<ConcreteExpansionEigenstrainBase>()
{
  InputParameters params = validParams<ComputeEigenstrainBase>();
  MooseEnum expansion_type("Isotropic Anisotropic");
  params.addParam<MooseEnum>(
      "expansion_type", expansion_type, "Type of expansion resulting from volumetric strain");
  params.addRangeCheckedParam<Real>(
      "compressive_strength", "compressive_strength > 0", "Compressive strength of concrete");
  params.addRangeCheckedParam<Real>(
      "expansion_stress_limit",
      "expansion_stress_limit > 0",
      "Upper bound compressive stress beyond which no expansion occurs");
  params.addRangeCheckedParam<Real>(
      "tensile_strength", "tensile_strength > 0", "Tensile strength of concrete");
  return params;
}

ConcreteExpansionEigenstrainBase::ConcreteExpansionEigenstrainBase(
    const InputParameters & parameters, const std::string volumetric_expansion_name)
  : ComputeEigenstrainBase(parameters),
    _expansion_type(getParam<MooseEnum>("expansion_type").getEnum<ExpansionType>()),
    _f_compress(isParamValid("compressive_strength") ? getParam<Real>("compressive_strength") : 0.0),
    _sigma_u(isParamValid("expansion_stress_limit") ? getParam<Real>("expansion_stress_limit") : 0.0),
    _f_tensile(isParamValid("tensile_strength") ? getParam<Real>("tensile_strength") : 0.0),
    _eigenstrain_old(getMaterialPropertyOld<RankTwoTensor>(_eigenstrain_name)),
    _volumetric_strain(declareProperty<Real>(volumetric_expansion_name + "_volumetric_strain")),
    _volumetric_strain_old(
        getMaterialPropertyOld<Real>(volumetric_expansion_name + "_volumetric_strain")),
    _stress(getMaterialPropertyOld<RankTwoTensor>("stress"))
{
  if (_expansion_type == ExpansionType::Anisotropic)
  {
    if (!parameters.isParamSetByUser("compressive_strength"))
      paramError("compressive_strength", "parameter is required for expansion_type = Anisotropic");
    if (!parameters.isParamSetByUser("expansion_stress_limit"))
      paramError("expansion_stress_limit", "parameter is required for expansion_type = Anisotropic");
    if (!parameters.isParamSetByUser("tensile_strength"))
      paramError("tensile_strength", "parameter is required for expansion_type = Anisotropic");
  }


  // Initialize triaxial weight table
  const Real third = 1.0 / 3.0;
  _triaxial_weights = {{{{third, third, 0.0, 0.0}}, //  0 -> 13
                        {{third, third, 0.0, 0.0}}, //  1 -> 14
                        {{0.5, 0.5, 0.0, 0.0}},     //  2 -> 15
                        {{0.5, 0.5, 0.0, 0.0}},     //  3 -> 16
                        {{third, third, 0.0, 0.0}}, //  4 -> 12
                        {{third, third, 0.0, 0.0}}, //  5 ->  1
                        {{0.5, 0.5, 0.0, 0.0}},     //  6 ->  2
                        {{0.5, 0.5, 0.0, 0.0}},     //  7 ->  5
                        {{0.5, 0.5, 0.0, 0.0}},     //  8 -> 11
                        {{0.5, 0.5, 0.0, 0.0}},     //  9 ->  4
                        {{1.0, 1.0, third, 0.0}},   // 10 ->  3
                        {{1.0, 1.0, 0.5, 0.0}},     // 11 ->  6
                        {{0.5, 0.5, 0.0, 0.0}},     // 12 -> 10
                        {{0.5, 0.5, 0.0, 0.0}},     // 13 ->  9
                        {{1.0, 1.0, 0.5, 0.0}},     // 14 ->  8
                        {{1.0, 1.0, 1.0, third}}}}; // 15 ->  7
}

void
ConcreteExpansionEigenstrainBase::initQpStatefulProperties()
{
  _volumetric_strain[_qp] = 0.0;
}

void
ConcreteExpansionEigenstrainBase::computeQpEigenstrain()
{
  if (_expansion_type == ExpansionType::Anisotropic || needStressEigenvalues())
    _stress[_qp].symmetricEigenvaluesEigenvectors(_stress_eigenvalues, _stress_eigenvectors);

  _volumetric_strain[_qp] = computeQpVolumetricStrain();

  if (_expansion_type == ExpansionType::Isotropic)
  {
    const Real eigenstrain_comp = computeVolumetricStrainComponent(_volumetric_strain[_qp]);
    _eigenstrain[_qp].zero();
    _eigenstrain[_qp].addIa(eigenstrain_comp);
  }
  else if (_expansion_type == ExpansionType::Anisotropic)
  {
    const Real inc_volumetric_strain = _volumetric_strain[_qp] - _volumetric_strain_old[_qp];

    RankTwoTensor inc_weighted_strain;

    Real sig_l = _stress_eigenvalues[0];
    Real sig_m = _stress_eigenvalues[1];
    Real sig_k = _stress_eigenvalues[2];

    // Weights
    // W_k
    Real W_3 = weight(sig_l, sig_m, sig_k);
    // W_m
    Real W_2 = weight(sig_k, sig_l, sig_m);
    // W_l
    Real W_1 = weight(sig_m, sig_k, sig_l);

    inc_weighted_strain(0, 0) = inc_volumetric_strain * W_1;
    inc_weighted_strain(1, 1) = inc_volumetric_strain * W_2;
    inc_weighted_strain(2, 2) = inc_volumetric_strain * W_3;

    // Rotate from principal coordinate system to Cartesian coordinate system
    inc_weighted_strain.rotate(_stress_eigenvectors);

    _eigenstrain[_qp] = _eigenstrain_old[_qp] + inc_weighted_strain;
  }
  else
    mooseError("Invalid expansion type");
}

Real
ConcreteExpansionEigenstrainBase::weight(Real sig_l, Real sig_m, Real sig_k)
{
  // Inputs
  const Real a1 = _f_tensile, a2 = -_sigma_u, a3 = -_f_compress + _sigma_u;
  const Real b1 = _f_tensile, b2 = -_sigma_u, b3 = -_f_compress + _sigma_u;

  // Lower and upper bound for l
  const unsigned int pbound_l = findNeighborIndex(sig_l);

  // Lower and upper bound for m
  const unsigned int pbound_m = findNeighborIndex(sig_m);

  // Neighboring nodes of k
  const unsigned int pbound_k = findNeighborIndex(sig_k);

  // Neighboring nodes of l, m, k
  const unsigned int N1 = (pbound_m)*4 + pbound_l;
  const unsigned int N2 = (pbound_m)*4 + (pbound_l + 1);
  const unsigned int N3 = (pbound_m + 1) * 4 + pbound_l;
  const unsigned int N4 = (pbound_m + 1) * 4 + (pbound_l + 1);

  const unsigned int N5 = pbound_k;
  const unsigned int N6 = pbound_k + 1;

  // Calculate a, b, sig_l, sig_m
  const Real a = computeAB(a1, a2, a3, pbound_l);
  const Real b = computeAB(b1, b2, b3, pbound_m);
  sig_l = computeSigma(sig_l, pbound_l);
  sig_m = computeSigma(sig_m, pbound_m);

  // Calculate weights
  return computeW(N1, N2, N3, N4, N5, N6, a, b, sig_l, sig_m, sig_k);
}

int
ConcreteExpansionEigenstrainBase::findNeighborIndex(Real sig)
{
  if (sig <= -_sigma_u)
    return 2;
  else if (sig > -_sigma_u && sig <= 0)
    return 1;
  else if (sig > 0)
    return 0;
  else
    mooseError("Invalid value for sig.");
}

Real
ConcreteExpansionEigenstrainBase::computeAB(const Real ab1,
                                            const Real ab2,
                                            const Real ab3,
                                            const unsigned int pbound)
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
ConcreteExpansionEigenstrainBase::computeSigma(const Real sig, const unsigned int pbound)
{
  if (pbound == 2)
  {
    if (sig < -_f_compress) // stress conditions beyond _f_compress
      return -_f_compress + _sigma_u;
    else
      return sig + _sigma_u;
  }
  else
  {
    if (sig > _f_tensile)
      return _f_tensile; // stress conditions beyond _f_tensile
    else
      return sig;
  }
}

Real
ConcreteExpansionEigenstrainBase::computeW(const unsigned int N1,
                                           const unsigned int N2,
                                           const unsigned int N3,
                                           const unsigned int N4,
                                           const unsigned int N5,
                                           const unsigned int N6,
                                           const Real a,
                                           const Real b,
                                           const Real sig_l,
                                           const Real sig_m,
                                           Real sig_k)
{
  if (sig_k < -_f_compress)
    sig_k = -_f_compress;
  else if (sig_k > _f_tensile)
    sig_k = _f_tensile;

  // Calculate weights
  // N1
  const Real W1 = computeWi(N1, N5, N6, sig_k);
  // N2
  const Real W2 = computeWi(N2, N5, N6, sig_k);
  // N3
  const Real W3 = computeWi(N3, N5, N6, sig_k);
  // N4
  const Real W4 = computeWi(N4, N5, N6, sig_k);

  //   local node numebers
  //   3-----4
  //   |     |
  //   |     |
  //   1-----2
  const Real WN1 = (1.0 / (a * b)) * (a - sig_l) * (b - sig_m);
  const Real WN2 = (1.0 / (a * b)) * sig_l * (b - sig_m);
  const Real WN3 = (1.0 / (a * b)) * (a - sig_l) * sig_m;
  const Real WN4 = (1.0 / (a * b)) * sig_l * sig_m;

  return W1 * WN1 + W2 * WN2 + W3 * WN3 + W4 * WN4;
}

Real
ConcreteExpansionEigenstrainBase::computeWi(const unsigned int N,
                                            const unsigned int N5,
                                            const unsigned int N6,
                                            const Real sig_k)
{
  const Real farr[4] = {_f_tensile, 0, -_sigma_u, -_f_compress};
  const Real W_1 = _triaxial_weights[N][N5];
  const Real W_2 = _triaxial_weights[N][N6];
  return W_1 + (W_2 - W_1) * (sig_k - farr[N5]) / (farr[N6] - farr[N5]);
}
