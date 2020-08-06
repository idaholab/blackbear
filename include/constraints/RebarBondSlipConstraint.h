//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

// MOOSE includes
#include "EqualValueEmbeddedConstraint.h"

// Forward Declarations
class RebarBondSlipConstraint;

template <>
InputParameters validParams<RebarBondSlipConstraint>();
/**
 * A RebarBondSlipConstraint enforces the constraint between concrete and
 * reinforcing bars establishing a slip vs. bondstress relationship
 */
class RebarBondSlipConstraint : public EqualValueEmbeddedConstraint
{
public:
  static InputParameters validParams();

  RebarBondSlipConstraint(const InputParameters & parameters);
  virtual void initialSetup() override;
  virtual void timestepSetup() override;
  bool shouldApply() override;
  void reinitConstraint();

  /**
   * Determine whether the coupled variable is one of the displacement variables,
   * and find its component
   * @param var_num The number of the variable to be checked
   * @param component The component index computed in this routine
   * @return bool indicating whether the coupled variable is one of the displacement variables
   */
  bool getCoupledVarComponent(unsigned int var_num, unsigned int & component);

protected:
  /// method to calculate the tangential and the normal direction for the rebars
  virtual void computeTangent();

  virtual Real computeQpResidual(Moose::ConstraintType type) override;
  virtual Real computeQpJacobian(Moose::ConstraintJacobianType type) override;
  virtual Real computeQpOffDiagJacobian(Moose::ConstraintJacobianType type,
                                        unsigned int jvar) override;

  /**
   * Struct designed to hold info about the bond-slip history
   * slip_min miminum slip value at the current step
   * slip_max maximum slip value at the current step
   * slip_min_old minimum slip value from the history
   * slip_max_old maximum slip value from the history
   * bondstress_min miminum bondstress value at the current step
   * bondstress_max maximum bondstress value at the current step
   * bondstress_min_old minimum bondstress value from the history
   * bondstress_max_old maximum bondstress value from the history
   */
  struct bondSlipData
  {
    Real slip_min;
    Real slip_max;
    Real slip_min_old;
    Real slip_max_old;
    Real bondstress_min;
    Real bondstress_max;
    Real bondstress_min_old;
    Real bondstress_max_old;

    /// initializing the bond-slip data
    bondSlipData()
      : slip_min(0.0),
        slip_max(0.0),
        slip_min_old(0.0),
        slip_max_old(0.0),
        bondstress_min(0.0),
        bondstress_max(0.0),
        bondstress_min_old(0.0),
        bondstress_max_old(0.0)
    {
    }
  };

  /// storing the bond-slip history values for each of the nodes
  std::map<dof_id_type, bondSlipData> _bondslip;

  /// the direction in which the constraint works
  const unsigned _component;

  /// problem dimesion
  const unsigned int _mesh_dimension;

  /// displacement variables
  std::vector<unsigned int> _var_nums;
  std::vector<MooseVariable *> _vars;

  /// flag to turn on printing values for debugging
  const bool _debug;

  /// maximum bond stress
  const Real _max_bondstress;

  /// residual bond stress due to friction after joint failure
  const Real _frictional_bondstress;

  /// ultimate slip value attainable before failure
  const Real _ultimate_slip;

  /// radius of the reinforcing bars
  const Real _bar_radius;

  /// slip values at the transition points of the bond-slip curve
  std::vector<Real> _transitional_slip;

  /// constraint force needed to enforce the constraint
  RealVectorValue _constraint_residual;

  /// constraint force needed to enforce the constraint
  RealVectorValue _constraint_jacobian_axial;

  /// penalty force for the current constraint
  RealVectorValue _pen_force;

  /// tangent direction for the rebars
  RealVectorValue _secondary_tangent;

  /// current element volume/length for the rabar
  Real _current_elem_volume;

  /// bond stress value
  Real _bond_stress;

  /// redivative of the bond stress function w.r.t slip
  Real _bond_stress_deriv;
};
