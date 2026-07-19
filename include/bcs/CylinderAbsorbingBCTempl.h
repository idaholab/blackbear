//* This file is part of BlackBear: https://github.com/idaholab/blackbear
//*
//* All rights reserved, see NOTICE.txt for full restrictions
//* https://github.com/idaholab/blackbear/blob/main/NOTICE.txt
//*
//* Get help at https://mooseframework.inl.gov/blackbear

#pragma once

#include "GenericIntegratedBC.h"

/**
 * Lysmer-Kuhlemeyer absorbing boundary for a smooth far-field surface of a
 * linear isotropic elastic domain. Applies the dynamic absorbing traction
 *
 *   t = -rho*c_p*(n.v)*n - rho*c_s*(v - (n.v)*n)
 *
 * one component at a time, in the style of DashpotBC. P- and S-wave speeds
 * are derived from material properties (density, shear modulus, Lame's first
 * parameter). Dynamic only: the residual collapses to zero without du/dt.
 */
template <bool is_ad>
class CylinderAbsorbingBCTempl : public GenericIntegratedBC<is_ad>
{
public:
  static InputParameters validParams();

  CylinderAbsorbingBCTempl(const InputParameters & parameters);

protected:
  GenericReal<is_ad> computeQpResidual() override;

  /// Cartesian component index (0=x, 1=y, 2=z) this BC instance acts on
  const int _component;

  /// Number of coupled displacement components
  const unsigned int _ndisp;

  /// Time derivatives of each coupled displacement, indexed by component
  std::vector<const GenericVariableValue<is_ad> *> _disp_dot;

  /// Mass density
  const MaterialProperty<Real> & _density;

  /// Shear modulus (mu)
  const MaterialProperty<Real> & _shear_modulus;

  /// First Lame parameter (lambda)
  const MaterialProperty<Real> & _lambda;

  usingGenericIntegratedBCMembers;
};

/**
 * Non-AD specialization that adds hand-coded Jacobian contributions. The AD
 * branch relies on the framework's automatic Jacobian assembly.
 */
class CylinderAbsorbingBC : public CylinderAbsorbingBCTempl<false>
{
public:
  static InputParameters validParams();

  CylinderAbsorbingBC(const InputParameters & parameters);

protected:
  Real computeQpJacobian() override;
  Real computeQpOffDiagJacobian(unsigned int jvar) override;

private:
  /// Coupled displacement variable numbers, indexed by component
  std::vector<unsigned int> _disp_var;
};

typedef CylinderAbsorbingBCTempl<true> ADCylinderAbsorbingBC;
