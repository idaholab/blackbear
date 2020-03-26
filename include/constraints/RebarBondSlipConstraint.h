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

/// A RebarBondSlipConstraint enforces concrete-rebar constraint
class RebarBondSlipConstraint : public EqualValueEmbeddedConstraint
{
public:
  static InputParameters validParams();

  RebarBondSlipConstraint(const InputParameters & parameters);
  virtual void initialSetup() override;
  virtual void timestepSetup() override;
  bool shouldApply() override;
  void reinitConstraint();

protected:
  virtual void computeTangent();
  virtual Real computeQpResidual(Moose::ConstraintType type) override;
  virtual Real computeQpJacobian(Moose::ConstraintJacobianType type) override;
  virtual Real computeQpOffDiagJacobian(Moose::ConstraintJacobianType type,
                                        unsigned int jvar) override;

  /**
   * Struct designed to hold info about the bonds slip history
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

  // Bond-slip data
  std::map<dof_id_type, bondSlipData> _bondslip;

  const unsigned _component;
  const unsigned int _mesh_dimension;
  std::vector<unsigned int> _var_nums;
  std::vector<MooseVariable *> _vars;

  const bool _debug;

  const Real _max_bondstress;
  const Real _frictional_bondstress;
  const Real _ultimate_slip;
  const Real _bar_radius;
  std::vector<Real> _transitional_slip;
  /// constraint force needed to enforce the constraint
  RealVectorValue _constraint_residual;
  /// penalty force for the current constraint
  RealVectorValue _pen_force;
  RealVectorValue _slave_tangent;
  Real _current_elem_volume;
  bool _bond;
  Real _bond_stress;
};
