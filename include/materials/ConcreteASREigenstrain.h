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

#ifndef CONCRETEASREIGENSTRAIN_H
#define CONCRETEASREIGENSTRAIN_H

#include "ConcreteExpansionEigenstrainBase.h"
#include "RankTwoTensor.h"

/**
 * Computes the volumetric expansion eigenstrain due to alkali-silica reaction.
 */

class ConcreteASREigenstrain : public ConcreteExpansionEigenstrainBase
{
public:
  ConcreteASREigenstrain(const InputParameters & parameters);

  void initQpStatefulProperties() override;
  bool needStressEigenvalues() override { return true; }
  Real computeQpVolumetricStrain() override;

  /**
   * Compute the residual for the computation of the ASR reaction extent.
   * @param qp Current quadrature point id
   * @param scalar Current iterative value of the ASR reaction extent
   * @return residual, which is the error in the new ASR reaction extent
   */
  Real computeResidual(unsigned qp, Real scalar);

  /**
   * Compute the derivative of the residual for the ASR reaction extent.
   * @param qp Current quadrature point id
   * @param scalar Current iterative value of the ASR reaction extent
   * @return derivative, which is the change in the error in the new ASR
   *         reaction extent wrt the reaction extent.
   */
  Real computeDerivative(unsigned qp, Real scalar);

private:
  /// Coupled value of temperature
  const VariableValue & _temperature;

  /// Coupled value of relative humidity
  const VariableValue & _rh;
  /// Power to which relative humidity is raised
  const Real & _rh_exponent;

  /// Final value of ASR volumetric expansion at full reaction extent
  Real _max_vol_expansion;
  /// Characteristic ASR time (days) at reference temperature
  Real _tau_c_T0;
  /// Latency ASR time (days) at reference temperature
  Real _tau_L_T0;
  /// Activation energy associated with tau_c
  Real _Uc;
  /// Activation energy associated with tau_L
  Real _UL;
  /// Reference temperature associated with ASR, converted to Kelvin
  Real _ref_temp;

  /// Constant for ASR latency time retardation under hydrostatic compression
  Real _alpha;

  ///@{ Parameters for ASR gel adsorption due to tensile cracking
  Real _gamma_tensile;
  Real _gamma_residual;
  ///@}

  /// Parameter for ASR gel adsorption due to compressive cracking
  Real _beta;

  /// Whether to include effect of ASR on tensile strength
  bool _ASR_dependent_tensile_strength;
  /// Fraction of tensile strength retained at full ASR reaction extent
  Real _beta_f;

  ///@{ Parameters controlling Newton iterations
  const unsigned int _max_its;
  const bool _output_iteration_info;
  const bool _output_iteration_info_on_error;
  const Real _relative_tolerance;
  const Real _absolute_tolerance;
  ///@}

  ///@{ Current and previous ASR reaction extent
  MaterialProperty<Real> & _ASR_extent;
  const MaterialProperty<Real> & _ASR_extent_old;
  ///@}

  /// Offset applied to convert temperature to Kelvin
  Real _temp_offset;
};

template <>
InputParameters validParams<ConcreteASREigenstrain>();

#endif // CONCRETEASREIGENSTRAIN_H
