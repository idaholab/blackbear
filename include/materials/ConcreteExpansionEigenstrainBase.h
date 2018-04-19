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

#ifndef CONCRETEEXPANSIONEIGENSTRAINBASE_H
#define CONCRETEEXPANSIONEIGENSTRAINBASE_H

#include "ComputeEigenstrainBase.h"
#include "RankTwoTensor.h"

#include <array>

class ConcreteExpansionEigenstrainBase;

template <>
InputParameters validParams<ConcreteExpansionEigenstrainBase>();

/**
 * ConcreteExpansionEigenstrainBase is a base class for computing the
 * eigenstrain tensor to represent the effect of expansive reactions
 * in concrete. A class that derives from this base class must define
 * the scalar volumetric expansion. This base class applies that
 * in either an isotropic or anisotropic manner.
 *
 * In the anisotropic case, the following technique is used:
 * V. Saouma and L. Perotti. Constitutive model for alkali-aggregate
 * reactions. ACI Materials Journal, 103(3), 2006.
 */
class ConcreteExpansionEigenstrainBase : public ComputeEigenstrainBase
{
public:
  ConcreteExpansionEigenstrainBase(const InputParameters & parameters,
                                   const std::string volumetric_expansion_name);

  void initQpStatefulProperties() override;
  void computeQpEigenstrain() override;

protected:
  /**
   * Compute the total volumetric strain due to a swelling reaction for
   * the current quadrature point.
   * @return volumetric strain
   */
  virtual Real computeQpVolumetricStrain() = 0;

  /**
   * Does the derived class need the eigenvalues of the stress tensor?
   * @return true if it does
   */
  virtual bool needStressEigenvalues() = 0;

  /// Enum used to specify type of expansion
  enum class ExpansionType
  {
    Isotropic,
    Anisotropic
  };
  /// Type of expansion
  const ExpansionType _expansion_type;

  /// Compressive strength of concrete (Pa) input and stored as a positive number
  const Real _f_compress;

  /// Upper bound compressive stress beyond which no expansion occurs (in Pascals)
  const Real _sigma_u;

  /// Tensile strength of concrete (Pa)
  const Real _f_tensile;

  /// Eigenvalues of stress tensor for current qp (computed only if needed)
  std::vector<Real> _stress_eigenvalues;

  /// Eigenvectors of stress tensor for current qp (computed only if needed)
  RankTwoTensor _stress_eigenvectors;

  /// Reference to old eigenstrain tensor
  const MaterialProperty<RankTwoTensor> & _eigenstrain_old;

  ///@{ Current and previous volumetric strains
  MaterialProperty<Real> & _volumetric_strain;
  const MaterialProperty<Real> & _volumetric_strain_old;
  ///@}

  /// Reference to stress tensor (computed outside this model)
  const MaterialProperty<RankTwoTensor> & _stress;

  // Triaxial weight table
  std::array<std::array<Real, 4>, 16> _triaxial_weights;

  /**
   * Compute the weight factor for anisotropic ASR volumetric strain
   * redistribution
   * @param sig_l  First principal stress
   * @param sig_m  Second principal stress
   * @param sig_k  Third principal stress
   * @return weight
   */
  Real weight(Real sig_l, Real sig_m, Real sig_k);

  /// Calculate N1, N2, N3, N4, N5, N6

  /**
   * Find the index for the node in the interpolation table
   * based on the stress.
   * @param sig  Stress
   * @return index
   */
  int findNeighborIndex(Real sig);

  /**
   * Determine the term of the A or B vector in Equation 17 of Saouma and Perotti
   * to be used
   * @param ab1 First component of a or b vector
   * @param ab2 Second component of a or b vector
   * @param ab3 Third component of a or b vector
   * @param pbound index used for selection of behavior
   * @return Value of selected entry in vector
   */
  Real computeAB(const Real ab1, const Real ab2, const Real ab3, const unsigned int pbound);

  /**
   * Compute sigma in equation 18 of Saouma and Perotti
   * @param sig stress
   * @param pbound index used for selection of behavior
   * @return sigma stress
   */
  Real computeSigma(const Real sig, const unsigned int pbound);

  /**
   * Compute interpolated weight
   * @param N1 shape function
   * @param N2 shape function
   * @param N3 shape function
   * @param N4 shape function
   * @param N5 shape function
   * @param N6 shape function
   * @param a a-parameter
   * @param b b-parameter
   * @param sig_l principal stress
   * @param sig_m principal stress
   * @return W
   */
  Real computeW(const unsigned int N1,
                const unsigned int N2,
                const unsigned int N3,
                const unsigned int N4,
                const unsigned int N5,
                const unsigned int N6,
                const Real a,
                const Real b,
                const Real sig_l,
                const Real sig_m,
                Real sig_k);

  /**
   * Compute ith component of W (equation 14 in Saouma and Perotti)
   * @param N1 shape function
   * @param N5 shape function
   * @param N6 shape function
   * @param sig_k sigma_k
   * @return W_i
   */
  Real
  computeWi(const unsigned int N1, const unsigned int N5, const unsigned int N6, const Real sig_k);
};

#endif // CONCRETEEXPANSIONEIGENSTRAINBASE_H
