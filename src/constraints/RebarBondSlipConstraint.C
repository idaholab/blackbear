//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/primary/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

// MOOSE includes
#include "MooseError.h"
#include "MooseTypes.h"
#include "RebarBondSlipConstraint.h"
#include "Assembly.h"
#include "SystemBase.h"
#include "FEProblem.h"
#include "MathUtils.h"
#include "MooseTypes.h"
#include "Coupleable.h"

#include "libmesh/string_to_enum.h"
#include "metaphysicl/raw_type.h"

registerMooseObject("BlackBearApp", RebarBondSlipConstraint);
registerMooseObject("BlackBearApp", ADRebarBondSlipConstraint);

template <bool is_ad>
InputParameters
RebarBondSlipConstraintTempl<is_ad>::validParams()
{
  InputParameters params = EqualValueEmbeddedConstraint::validParams();
  params.addClassDescription(
      "This is a constraint enforcing the bod-slip behavior between concrete and rebar");
  params.addRequiredParam<unsigned int>("component",
                                        "An integer corresponding to the direction "
                                        "the variable this kernel acts in. (0 for x, "
                                        "1 for y, 2 for z)");
  params.addRequiredCoupledVar(
      "displacements",
      "The displacements appropriate for the simulation geometry and coordinate system");
  params.addRequiredParam<Real>("max_bondstress", "Maximum bond stress");
  params.addRequiredParam<Real>("transitional_slip_value",
                                "Transition between loading and frictional slip");
  params.addRequiredParam<Real>("rebar_radius", "Radius of the rebar");

  params.addCoupledVar("output_bond_slip", "Bond slip output variable.");
  params.addCoupledVar("output_bond_force", "Bond force output variable");
  params.addCoupledVar("output_bond_slip_type", "Bond slip type output variable");
  return params;
}

template <bool is_ad>
RebarBondSlipConstraintTempl<is_ad>::RebarBondSlipConstraintTempl(
    const InputParameters & parameters)
  : EqualValueEmbeddedConstraintTempl<is_ad>(parameters),
    _component(this->template getParam<unsigned int>("component")),
    _mesh_dimension(_mesh.dimension()),
    _var_nums(_mesh_dimension, libMesh::invalid_uint),
    _vars(_mesh_dimension, nullptr),
    _max_bondstress(this->template getParam<Real>("max_bondstress")),
    _bar_radius(this->template getParam<Real>("rebar_radius")),
    _transitional_slip(this->template getParam<Real>("transitional_slip_value"))
{
  if (_mesh_dimension != this->coupledComponents("displacements"))
    mooseError("In RebarBondSlipConstraint, number of displacements must equal the mesh dimension");

  for (unsigned int i = 0; i < _mesh_dimension; ++i)
  {
    _var_nums[i] = this->coupled("displacements", i);
    _vars[i] = this->getVar("displacements", i);
  }

  if (this->isParamValid("output_bond_slip"))
    _output_bond_slip = &(this->writableVariable("output_bond_slip"));
  if (this->isParamValid("output_bond_force"))
    _output_bond_force = &(this->writableVariable("output_bond_force"));
  if (this->isParamValid("output_bond_slip_type"))
    _output_bond_slip_type = &(this->writableVariable("output_bond_slip_type"));
}

template <bool is_ad>
void
RebarBondSlipConstraintTempl<is_ad>::initialSetup()
{
  for (auto it = _secondary_to_primary_map.begin(); it != _secondary_to_primary_map.end(); ++it)
    if (_bondslip.find(it->first) == _bondslip.end())
      _bondslip.insert(std::pair<dof_id_type, bondSlipData>(it->first, bondSlipData()));
}

template <bool is_ad>
void
RebarBondSlipConstraintTempl<is_ad>::timestepSetup()
{
  for (auto iter = _secondary_to_primary_map.begin(); iter != _secondary_to_primary_map.end();
       ++iter)
  {
    dof_id_type node_id = iter->first;
    [[maybe_unused]] auto it = _bondslip.find(node_id);
    mooseAssert(it != _bondslip.end(), "Node not found in bond-slip map");

    _bondslip[node_id].slip_min_old = _bondslip[node_id].slip_min;
    _bondslip[node_id].slip_max_old = _bondslip[node_id].slip_max;
    _bondslip[node_id].bondstress_min_old = _bondslip[node_id].bondstress_min;
    _bondslip[node_id].bondstress_max_old = _bondslip[node_id].bondstress_max;
  }
}

template <bool is_ad>
bool
RebarBondSlipConstraintTempl<is_ad>::shouldApply()
{
  auto it = _secondary_to_primary_map.find(_current_node->id());

  if (it != _secondary_to_primary_map.end())
  {
    const Elem * primary_elem = _mesh.elemPtr(it->second);
    std::vector<Point> points = {*_current_node};

    // reinit variables on the primary element at the secondary point
    this->_fe_problem.setNeighborSubdomainID(primary_elem, 0);
    this->_fe_problem.reinitNeighborPhys(primary_elem, points, 0);

    reinitConstraint();

    return true;
  }
  return false;
}

template <bool is_ad>
void
RebarBondSlipConstraintTempl<is_ad>::computeTangent()
{
  _secondary_node_tangent = 0.0;
  _secondary_node_length = 0.0;
  // get normals
  // get connected elements of the current node
  const std::map<dof_id_type, std::vector<dof_id_type>> & node_to_elem_map = _mesh.nodeToElemMap();
  auto node_to_elem_pair = node_to_elem_map.find(_current_node->id());
  mooseAssert(node_to_elem_pair != node_to_elem_map.end(), "Missing entry in node to elem map");
  const std::vector<dof_id_type> & elems = node_to_elem_pair->second;
  for (auto & elem : elems)
  {
    const Elem * elem_ptr = _mesh.elemPtr(elem);
    mooseAssert(elem_ptr->dim() == 1, "Elements attached to secondary node must be 1D.");
    this->_assembly.reinit(elem_ptr);
    _secondary_node_length += elem_ptr->volume();
    const std::vector<RealGradient> * tangents =
        &_subproblem.assembly(Constraint::_tid, _sys.number()).getFE(FEType(), 1)->get_dxyzdxi();
    for (std::size_t i = 0; i < tangents->size(); i++)
    {
      // need to properly integrate this.  This only works for a single integration point
      _secondary_node_tangent += (*tangents)[i];
    }
  }

  _secondary_node_tangent = _secondary_node_tangent / _secondary_node_tangent.norm();
  _secondary_node_length = _secondary_node_length / elems.size();
}

template <>
RealVectorValue
RebarBondSlipConstraintTempl<false>::computeRelativeDisplacement()
{
  RealVectorValue relative_disp;
  for (unsigned int i = 0; i < _mesh_dimension; ++i)
    relative_disp(i) = ((_vars[i]->dofValues())[0] - (_vars[i]->slnNeighbor())[0]);
  return relative_disp;
}

template <>
ADRealVectorValue
RebarBondSlipConstraintTempl<true>::computeRelativeDisplacement()
{
  ADRealVectorValue relative_disp;
  for (unsigned int i = 0; i < _mesh_dimension; ++i)
    relative_disp(i) = ((_vars[i]->adDofValues())[0] - (_vars[i]->adSlnNeighbor())[0]);
  return relative_disp;
}

template <bool is_ad>
void
RebarBondSlipConstraintTempl<is_ad>::reinitConstraint()
{
  computeTangent();

  // Build up residual vector
  GenericRealVectorValue<is_ad> relative_disp = computeRelativeDisplacement();

  GenericReal<is_ad> slip = relative_disp * _secondary_node_tangent;
  GenericRealVectorValue<is_ad> slip_axial = slip * _secondary_node_tangent;
  GenericRealVectorValue<is_ad> slip_normal = relative_disp - slip_axial;

  const Node * node = _current_node;
  auto it = _bondslip.find(node->id());
  mooseAssert(it != _bondslip.end(), "Node not found in bond-slip map");
  bondSlipData bond_slip = it->second;

  const Real slip_min = std::min(bond_slip.slip_min_old, MetaPhysicL::raw_value(slip));
  const Real slip_max = std::max(bond_slip.slip_max_old, MetaPhysicL::raw_value(slip));

  GenericReal<is_ad> slope = _max_bondstress / _transitional_slip;
  GenericReal<is_ad> plastic_slip_max = slip_max - bond_slip.bondstress_max_old / slope;
  GenericReal<is_ad> plastic_slip_min = slip_min - bond_slip.bondstress_min_old / slope;

  _bond_stress_deriv = 0.0;
  _bond_stress = 0.0;
  Real slip_type = 0;
  if (slip >= slip_max || slip <= slip_min)
  {
    if (std::abs(slip) < _transitional_slip)
    {
      // elastic load or unload
      slip_type = 1;
      _bond_stress = slip * slope;
      _bond_stress_deriv = slope;
    }
    else
    {
      // plastic slip
      slip_type = 2;
      _bond_stress = MathUtils::sign(slip) * _max_bondstress;
    }
  }
  else if (slip > plastic_slip_max && slip < bond_slip.slip_max)
  {
    // unload after positive plastic slip
    slip_type = 4;
    _bond_stress = (slip - plastic_slip_max) * bond_slip.bondstress_max /
                   (bond_slip.slip_max - plastic_slip_max);
    _bond_stress_deriv = slope;
  }
  else if (slip < plastic_slip_min && slip > bond_slip.slip_min)
  {
    // unload after negative plastic slip
    slip_type = 5;
    _bond_stress = (slip - plastic_slip_min) * bond_slip.bondstress_min /
                   (bond_slip.slip_min - plastic_slip_min);
    _bond_stress_deriv = slope;
  }
  else
  {
    // slip at zero stress
    slip_type = 6;
    _bond_stress = 0;
    _bond_stress_deriv = 0;
  }

  GenericReal<is_ad> bond_force =
      2.0 * libMesh::pi * _bar_radius * _secondary_node_length * _bond_stress;
  GenericReal<is_ad> bond_force_deriv =
      2.0 * libMesh::pi * _bar_radius * _secondary_node_length * _bond_stress_deriv;

  GenericRealVectorValue<is_ad> constraint_force_axial = bond_force * _secondary_node_tangent;
  GenericRealVectorValue<is_ad> constraint_force_normal = this->_penalty * slip_normal;

  _constraint_residual = constraint_force_axial + constraint_force_normal;
  _constraint_jacobian_axial = bond_force_deriv * _secondary_node_tangent;

  const Real bondstress_min =
      std::min(bond_slip.bondstress_min_old, MetaPhysicL::raw_value(_bond_stress));
  const Real bondstress_max =
      std::max(bond_slip.bondstress_max_old, MetaPhysicL::raw_value(_bond_stress));

  _bondslip[node->id()].slip_min = slip_min;
  _bondslip[node->id()].slip_max = slip_max;
  _bondslip[node->id()].bondstress_max = bondstress_max;
  _bondslip[node->id()].bondstress_min = bondstress_min;

  if (_output_bond_slip)
    _output_bond_slip->setNodalValue(MetaPhysicL::raw_value(slip));
  if (_output_bond_force)
    _output_bond_force->setNodalValue(MetaPhysicL::raw_value(_bond_stress));
  if (_output_bond_slip_type)
    _output_bond_slip_type->setNodalValue(slip_type);
}

template <bool is_ad>
GenericReal<is_ad>
RebarBondSlipConstraintTempl<is_ad>::computeQpResidual(Moose::ConstraintType type)
{
  GenericReal<is_ad> resid = _constraint_residual(_component);
  switch (type)
  {
    case Moose::Secondary:
      return resid * _test_secondary[_i][_qp];

    case Moose::Primary:
      return -resid * _test_primary[_i][_qp];
  }

  return 0.0;
}

template <>
Real
RebarBondSlipConstraintTempl<true>::computeQpJacobian(Moose::ConstraintJacobianType /*type*/)
{
  mooseError(
      "In RebarBondSlipConstraint, computeQpJacobian should not be called. Check computeQpJacobian "
      "implementation.");
  return 0.0;
}

template <>
Real
RebarBondSlipConstraintTempl<false>::computeQpJacobian(Moose::ConstraintJacobianType type)
{

  Real jac_axial = _constraint_jacobian_axial(_component);

  switch (type)
  {
    case Moose::SecondarySecondary:
      return _phi_secondary[_j][_qp] * jac_axial * _secondary_node_tangent(_component) *
                 _test_secondary[_i][_qp] +
             _phi_secondary[_j][_qp] * this->_penalty * _test_secondary[_i][_qp] *
                 (1.0 - _secondary_node_tangent(_component) * _secondary_node_tangent(_component));

    case Moose::SecondaryPrimary:
      return -_phi_primary[_j][_qp] * jac_axial * _secondary_node_tangent(_component) *
                 _test_secondary[_i][_qp] -
             _phi_primary[_j][_qp] * this->_penalty * _test_secondary[_i][_qp] *
                 (1.0 - _secondary_node_tangent(_component) * _secondary_node_tangent(_component));

    case Moose::PrimarySecondary:
      return -_test_primary[_i][_qp] * jac_axial * _secondary_node_tangent(_component) *
                 _phi_secondary[_j][_qp] -
             _test_primary[_i][_qp] * this->_penalty * _phi_secondary[_j][_qp] *
                 (1.0 - _secondary_node_tangent(_component) * _secondary_node_tangent(_component));

    case Moose::PrimaryPrimary:
      return _test_primary[_i][_qp] * jac_axial * _secondary_node_tangent(_component) *
                 _phi_primary[_j][_qp] +
             _test_primary[_i][_qp] * this->_penalty * _phi_primary[_j][_qp] *
                 (1.0 - _secondary_node_tangent(_component) * _secondary_node_tangent(_component));

    default:
      mooseError("Unsupported type");
      break;
  }
  return 0.0;
}

template <>
Real
RebarBondSlipConstraintTempl<true>::computeQpOffDiagJacobian(Moose::ConstraintJacobianType /*type*/,
                                                             unsigned int /*jvar*/)
{
  mooseError("In RebarBondSlipConstraint, computeQpOffDiagJacobian should not be called. Check "
             "computeQpJacobian implementation.");
  return 0.0;
}

template <>
Real
RebarBondSlipConstraintTempl<false>::computeQpOffDiagJacobian(Moose::ConstraintJacobianType type,
                                                              unsigned int jvar)
{
  Real jac_axial = _constraint_jacobian_axial(_component);

  unsigned int coupled_component;
  Real tangent_component_in_coupled_var_dir = 1.0;
  if (getCoupledVarComponent(jvar, coupled_component))
    tangent_component_in_coupled_var_dir = _secondary_node_tangent(coupled_component);

  switch (type)
  {
    case Moose::SecondarySecondary:
      return _phi_secondary[_j][_qp] * jac_axial * tangent_component_in_coupled_var_dir *
                 _test_secondary[_i][_qp] -
             _phi_secondary[_j][_qp] * this->_penalty * _test_secondary[_i][_qp] *
                 _secondary_node_tangent(_component) * tangent_component_in_coupled_var_dir;

    case Moose::SecondaryPrimary:
      return -_phi_primary[_j][_qp] * jac_axial * tangent_component_in_coupled_var_dir *
                 _test_secondary[_i][_qp] +
             _phi_primary[_j][_qp] * this->_penalty * _test_secondary[_i][_qp] *
                 _secondary_node_tangent(_component) * tangent_component_in_coupled_var_dir;

    case Moose::PrimarySecondary:
      return -_test_primary[_i][_qp] * jac_axial * tangent_component_in_coupled_var_dir *
                 _phi_secondary[_j][_qp] +
             _test_primary[_i][_qp] * this->_penalty * _phi_secondary[_j][_qp] *
                 _secondary_node_tangent(_component) * tangent_component_in_coupled_var_dir;

    case Moose::PrimaryPrimary:
      return _test_primary[_i][_qp] * jac_axial * tangent_component_in_coupled_var_dir *
                 _phi_primary[_j][_qp] -
             _test_primary[_i][_qp] * this->_penalty * _phi_primary[_j][_qp] *
                 _secondary_node_tangent(_component) * tangent_component_in_coupled_var_dir;

    default:
      mooseError("Unsupported type");
      break;
  }
  return 0.0;
}

template <bool is_ad>
bool
RebarBondSlipConstraintTempl<is_ad>::getCoupledVarComponent(unsigned int var_num,
                                                            unsigned int & component)
{
  component = std::numeric_limits<unsigned int>::max();
  bool coupled_var_is_disp_var = false;
  for (unsigned int i = 0; i < LIBMESH_DIM; ++i)
    if (var_num == _var_nums[i])
    {
      coupled_var_is_disp_var = true;
      component = i;
      break;
    }

  return coupled_var_is_disp_var;
}

template class RebarBondSlipConstraintTempl<false>;
template class RebarBondSlipConstraintTempl<true>;
