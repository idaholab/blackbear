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
#include "Coupleable.h"
#include "EqualValueEmbeddedConstraint.h"
#include "Moose.h"

/**
 * A RebarBondSlipConstraint enforces the constraint between concrete and
 * reinforcing bars establishing a slip vs. bondstress relationship
 */
template <bool is_ad>
class RebarBondSlipConstraintTempl : public EqualValueEmbeddedConstraintTempl<is_ad>
{
public:
  static InputParameters validParams();

  RebarBondSlipConstraintTempl(const InputParameters & parameters);
  virtual void initialSetup() override;
  virtual void timestepSetup() override;

  bool shouldApply() override;
  void reinitConstraint() override;

  /**
   * Determine whether the coupled variable is one of the displacement variables,
   * and find its component
   * @param var_num The number of the variable to be checked
   * @param component The component index computed in this routine
   * @return bool indicating whether the coupled variable is one of the displacement variables
   */
  bool getCoupledVarComponent(unsigned int var_num, unsigned int & component);

protected:
  virtual GenericReal<is_ad> computeQpResidual(Moose::ConstraintType type) override;
  virtual Real computeQpJacobian(Moose::ConstraintJacobianType type) override;
  virtual Real computeQpOffDiagJacobian(Moose::ConstraintJacobianType type,
                                        unsigned int jvar) override;

  /// method to calculate the tangential and the normal direction for the rebars
  virtual void computeTangent();

  /// node element constraint bond slip model
  const enum class BondSlipModel {
    CONCRETE_REBAR_MODEL,
    ELASTIC_PERFECT_PLASTIC_MODEL
  } _bondslip_model;

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

  /// constraint model with elastic perfect plastic force slip model
  void elasticPerfectPlasticModel(const GenericReal<is_ad> slip,
                                  const bondSlipData * const bond_slip,
                                  GenericReal<is_ad> & bond_stress,
                                  GenericReal<is_ad> & bond_stress_deriv,
                                  Real & plastic_slip) const;

  /// constraint model for concrete rebar from eqn 11 in Mehlhorn 1983.
  void concreteRebarModel(const GenericReal<is_ad> slip,
                          const bondSlipData * const bond_slip,
                          GenericReal<is_ad> & bond_stress,
                          GenericReal<is_ad> & bond_stress_deriv,
                          Real & plastic_slip) const;

  /// component of the displacements in which the constraint works
  unsigned int _component;

  /// problem dimesion
  const unsigned int _mesh_dimension;

  /// displacement variables
  std::vector<unsigned int> _disp_vars_nums;
  std::vector<MooseVariable *> _disp_vars;

  /// maximum bond stress
  const Real _max_bondstress;

  /// radius of the reinforcing bars
  const Real _bar_radius;

  /// slip values at the transition points of the bond-slip curve
  const Real _transitional_slip;

  /// constraint force needed to enforce the constraint
  GenericRealVectorValue<is_ad> _constraint_residual;

  /// constraint force needed to enforce the constraint
  GenericRealVectorValue<is_ad> _constraint_jacobian_axial;

  /// tangent direction for the rebars
  GenericRealVectorValue<is_ad> _secondary_node_tangent;

  /// current element volume/length for the rabar
  GenericReal<is_ad> _secondary_node_length;

  /// bond stress value
  GenericReal<is_ad> _bond_stress;

  /// redivative of the bond stress function w.r.t slip
  GenericReal<is_ad> _bond_stress_deriv;

  // Optional Variable output of bond constraint data
  MooseWritableVariable * _output_axial_slip = nullptr;
  MooseWritableVariable * _output_axial_force = nullptr;
  MooseWritableVariable * _output_axial_plastic_slip = nullptr;

  usingGenericNodeElemConstraint;

private:
  GenericRealVectorValue<is_ad> computeRelativeDisplacement();
};

typedef RebarBondSlipConstraintTempl<false> RebarBondSlipConstraint;
typedef RebarBondSlipConstraintTempl<true> ADRebarBondSlipConstraint;
