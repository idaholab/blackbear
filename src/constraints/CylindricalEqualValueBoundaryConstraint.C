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

#include "CylindricalEqualValueBoundaryConstraint.h"

#include "Assembly.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "SystemBase.h"

#include "libmesh/int_range.h"
#include "libmesh/node.h"

registerMooseObject("BlackBearApp", CylindricalEqualValueBoundaryConstraint);

InputParameters
CylindricalEqualValueBoundaryConstraint::validParams()
{
  InputParameters params = EqualValueBoundaryConstraint::validParams();
  params.addClassDescription(
      "Penalty constraint enforcing equal radial displacement and zero circumferential "
      "displacement on all nodes of a curved cylindrical surface, relative to a primary "
      "node on the same surface. Couples the two in-plane Cartesian displacement "
      "components via a per-secondary rotation matrix.");
  params.addRequiredCoupledVar(
      "displacements",
      "The three Cartesian displacement variables in order 'disp_x disp_y disp_z'.");
  MooseEnum cylindrical_axis("x y z", "z");
  params.addParam<MooseEnum>(
      "cylindrical_axis", cylindrical_axis, "The Cartesian symmetry axis of the cylinder.");
  return params;
}

CylindricalEqualValueBoundaryConstraint::CylindricalEqualValueBoundaryConstraint(
    const InputParameters & parameters)
  : EqualValueBoundaryConstraint(parameters),
    _ax_idx(getParam<MooseEnum>("cylindrical_axis")),
    _r1_idx((_ax_idx + 1) % 3),
    _r2_idx((_ax_idx + 2) % 3),
    _disp_r2(getVar("displacements", _r2_idx))
{
  if (coupledComponents("displacements") != 3)
    paramError("displacements", "Exactly three displacement variables must be supplied.");

  if (_var.number() != getVar("displacements", _r1_idx)->number())
    paramError("variable",
               "'variable' must be set to the in-plane displacement at index ",
               _r1_idx,
               " of 'displacements' for cylindrical_axis = ",
               getParam<MooseEnum>("cylindrical_axis"),
               ".");

  computeAngles();
}

void
CylindricalEqualValueBoundaryConstraint::meshChanged()
{
  EqualValueBoundaryConstraint::meshChanged();
  computeAngles();
}

void
CylindricalEqualValueBoundaryConstraint::computeAngles()
{
  _cos_dtheta.clear();
  _sin_dtheta.clear();
  _cos_dtheta.reserve(_connected_nodes.size());
  _sin_dtheta.reserve(_connected_nodes.size());

  if (_primary_node_vector.empty())
    return;

  const Node & primary_node = _mesh.nodeRef(_primary_node_vector[0]);
  const Real x_p = primary_node(_r1_idx);
  const Real y_p = primary_node(_r2_idx);
  const Real R_p = std::sqrt(x_p * x_p + y_p * y_p);
  if (R_p == 0.0)
    mooseError("Primary node lies on the symmetry axis (r = 0); the cylindrical constraint "
               "is undefined there.");

  for (const auto & nid : _connected_nodes)
  {
    const Node & secondary_node = _mesh.nodeRef(nid);
    const Real x_s = secondary_node(_r1_idx);
    const Real y_s = secondary_node(_r2_idx);
    const Real R_s = std::sqrt(x_s * x_s + y_s * y_s);
    if (R_s == 0.0)
      mooseError("Secondary node ", nid, " lies on the symmetry axis (r = 0).");

    const Real inv = 1.0 / (R_s * R_p);
    _cos_dtheta.push_back((x_s * x_p + y_s * y_p) * inv);
    _sin_dtheta.push_back((y_s * x_p - x_s * y_p) * inv);
  }
}

Real
CylindricalEqualValueBoundaryConstraint::computeQpResidual(Moose::ConstraintType /*type*/)
{
  mooseError("CylindricalEqualValueBoundaryConstraint overrides the vector-arg computeResidual; "
             "computeQpResidual should never be called.");
}

Real
CylindricalEqualValueBoundaryConstraint::computeQpJacobian(Moose::ConstraintJacobianType /*type*/)
{
  mooseError("CylindricalEqualValueBoundaryConstraint overrides the vector-arg computeJacobian; "
             "computeQpJacobian should never be called.");
}

void
CylindricalEqualValueBoundaryConstraint::computeResidual(const NumericVector<Number> & /*residual*/)
{
  const auto & primary_dof_r1 = _var.dofIndices();
  const auto & primary_dof_r2 = _disp_r2->dofIndices();
  const auto & secondary_dof_r1 = _var.dofIndicesNeighbor();
  const auto & secondary_dof_r2 = _disp_r2->dofIndicesNeighbor();

  if (primary_dof_r1.empty() || secondary_dof_r1.empty())
    return;

  const Real u_r1_p = _var.dofValues()[0];
  const Real u_r2_p = _disp_r2->dofValues()[0];

  const auto & u_r1_s = _var.dofValuesNeighbor();
  const auto & u_r2_s = _disp_r2->dofValuesNeighbor();

  const std::size_t n = secondary_dof_r1.size();
  mooseAssert(n == _cos_dtheta.size(),
              "Cached angle table size does not match secondary node count.");

  DenseVector<Number> re_r1(1);
  DenseVector<Number> re_r2(1);
  DenseVector<Number> neighbor_re_r1(n);
  DenseVector<Number> neighbor_re_r2(n);

  for (const auto i : index_range(secondary_dof_r1))
  {
    const Real c = _cos_dtheta[i];
    const Real s = _sin_dtheta[i];

    const Real r0 = u_r1_s[i] - (u_r1_p * c - u_r2_p * s);
    const Real r1 = u_r2_s[i] - (u_r1_p * s + u_r2_p * c);

    neighbor_re_r1(i) = _penalty * r0;
    neighbor_re_r2(i) = _penalty * r1;

    re_r1(0) += _penalty * (u_r1_p - c * u_r1_s[i] - s * u_r2_s[i]);
    re_r2(0) += _penalty * (u_r2_p + s * u_r1_s[i] - c * u_r2_s[i]);
  }

  addResiduals(_assembly, re_r1, primary_dof_r1, _var.scalingFactor());
  addResiduals(_assembly, re_r2, primary_dof_r2, _disp_r2->scalingFactor());
  addResiduals(_assembly, neighbor_re_r1, secondary_dof_r1, _var.scalingFactor());
  addResiduals(_assembly, neighbor_re_r2, secondary_dof_r2, _disp_r2->scalingFactor());
}

void
CylindricalEqualValueBoundaryConstraint::computeJacobian(const SparseMatrix<Number> & /*jacobian*/)
{
  const auto & primary_dof_r1 = _var.dofIndices();
  const auto & primary_dof_r2 = _disp_r2->dofIndices();
  const auto & secondary_dof_r1 = _var.dofIndicesNeighbor();
  const auto & secondary_dof_r2 = _disp_r2->dofIndicesNeighbor();

  if (primary_dof_r1.empty() || secondary_dof_r1.empty())
    return;

  const dof_id_type p_r1 = primary_dof_r1[0];
  const dof_id_type p_r2 = primary_dof_r2[0];

  const Real sf_r1 = _var.scalingFactor();
  const Real sf_r2 = _disp_r2->scalingFactor();

  mooseAssert(secondary_dof_r1.size() == _cos_dtheta.size(),
              "Cached angle table size does not match secondary node count.");

  for (const auto i : index_range(secondary_dof_r1))
  {
    const Real c = _cos_dtheta[i];
    const Real s = _sin_dtheta[i];
    const dof_id_type s_r1 = secondary_dof_r1[i];
    const dof_id_type s_r2 = secondary_dof_r2[i];

    addJacobianElement(_assembly, _penalty, s_r1, s_r1, sf_r1);
    addJacobianElement(_assembly, _penalty, s_r2, s_r2, sf_r2);

    addJacobianElement(_assembly, -c * _penalty, s_r1, p_r1, sf_r1);
    addJacobianElement(_assembly, s * _penalty, s_r1, p_r2, sf_r1);
    addJacobianElement(_assembly, -s * _penalty, s_r2, p_r1, sf_r2);
    addJacobianElement(_assembly, -c * _penalty, s_r2, p_r2, sf_r2);

    addJacobianElement(_assembly, _penalty, p_r1, p_r1, sf_r1);
    addJacobianElement(_assembly, _penalty, p_r2, p_r2, sf_r2);

    addJacobianElement(_assembly, -c * _penalty, p_r1, s_r1, sf_r1);
    addJacobianElement(_assembly, -s * _penalty, p_r1, s_r2, sf_r1);
    addJacobianElement(_assembly, s * _penalty, p_r2, s_r1, sf_r2);
    addJacobianElement(_assembly, -c * _penalty, p_r2, s_r2, sf_r2);
  }
}
