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

#include "EqualValueBoundaryConstraint.h"

/**
 * Penalty NodalConstraint that ties all secondary nodes on a curved cylindrical
 * surface to a primary node by enforcing equal radial displacement and zero
 * circumferential displacement. Works on a Cartesian-aligned axis (x, y, or z);
 * the in-plane components are coupled via a 2x2 rotation matrix derived from
 * each secondary node's azimuthal offset relative to the primary.
 */
class CylindricalEqualValueBoundaryConstraint : public EqualValueBoundaryConstraint
{
public:
  static InputParameters validParams();

  CylindricalEqualValueBoundaryConstraint(const InputParameters & parameters);

  virtual void meshChanged() override;

  virtual void computeResidual(const NumericVector<Number> & residual) override;
  virtual void computeJacobian(const SparseMatrix<Number> & jacobian) override;

protected:
  /**
   * The QP hooks are unusable for this object (single-scalar _var, diagonal-only
   * Kne block, and the base's two-list dispatch instead of the four-list
   * dispatch needed here). Both vector-arg overloads above are overridden
   * directly; these stubs satisfy the pure-virtual contract on NodalConstraint.
   */
  virtual Real computeQpResidual(Moose::ConstraintType type) override;
  virtual Real computeQpJacobian(Moose::ConstraintJacobianType type) override;

  /**
   * Recompute the cached (cos(dtheta), sin(dtheta)) table for the current
   * primary and _connected_nodes. Called from the constructor and from
   * meshChanged(). Errors out if any secondary node lies on the symmetry axis.
   */
  void computeAngles();

  /// Cartesian index of the symmetry axis (0, 1, or 2)
  const unsigned int _ax_idx;
  /// Cartesian index of the first in-plane direction ((_ax_idx + 1) % 3)
  const unsigned int _r1_idx;
  /// Cartesian index of the second in-plane direction ((_ax_idx + 2) % 3)
  const unsigned int _r2_idx;

  /// The second in-plane displacement variable (the first is the inherited _var)
  MooseVariable * const _disp_r2;

  /// cos(theta_secondary - theta_primary), indexed by position in _connected_nodes
  std::vector<Real> _cos_dtheta;
  /// sin(theta_secondary - theta_primary), indexed by position in _connected_nodes
  std::vector<Real> _sin_dtheta;
};
