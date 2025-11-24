//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/primary/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

// MOOSE includes
#include "MooseArray.h"
#include "MooseError.h"
#include "MooseTypes.h"
#include "RebarBondSlipConstraint.h"
#include "Assembly.h"
#include "SystemBase.h"
#include "FEProblem.h"
#include "MathUtils.h"
#include "MooseTypes.h"
#include "Coupleable.h"
#include "Executioner.h"

#include "metaphysicl/raw_type.h"
#include "libmesh/utility.h"

registerMooseObject("BlackBearApp", RebarBondSlipConstraint);
registerMooseObject("BlackBearApp", ADRebarBondSlipConstraint);

template <bool is_ad>
InputParameters
RebarBondSlipConstraintTempl<is_ad>::validParams()
{
  InputParameters params = EqualValueEmbeddedConstraint::validParams();
  params.addClassDescription("Constraint using a physical model to enforce the bond-slip behavior "
                             "between concrete and rebar.");
  params.addRequiredCoupledVar(
      "displacements",
      "The displacements appropriate for the simulation geometry and coordinate system");
  params.addRequiredParam<Real>("max_bond_stress", "Maximum bond stress");
  params.setDocUnit("max_bond_stress", "stress");
  params.addRequiredParam<Real>("transitional_slip_value",
                                "Transition between loading and frictional slip");
  params.setDocUnit("transitional_slip_value", "length");
  params.addRequiredParam<Real>("rebar_radius", "Radius of the rebar");
  params.setDocUnit("rebar_radius", "length");
  MooseEnum bondslip_model("concrete_rebar_model elastic_perfect_plastic_model",
                           "elastic_perfect_plastic_model");
  params.addParam<MooseEnum>("bondslip_model", bondslip_model, "Node-Element bond slip model.");

  params.addCoupledVar("output_axial_slip", "Rebar axial slip output variable.");
  params.addCoupledVar("output_axial_force", "Rebar axial force output variable");
  params.addCoupledVar("output_axial_plastic_slip", "Rebar axial plastic component of bond slip");
  return params;
}

template <bool is_ad>
RebarBondSlipConstraintTempl<is_ad>::RebarBondSlipConstraintTempl(
    const InputParameters & parameters)
  : EqualValueEmbeddedConstraintTempl<is_ad>(parameters),
    _bondslip_model(
        this->template getParam<MooseEnum>("bondslip_model").template getEnum<BondSlipModel>()),
    _component(libMesh::invalid_uint),
    _mesh_dimension(_mesh.dimension()),
    _disp_vars_nums(_mesh_dimension, libMesh::invalid_uint),
    _disp_vars(_mesh_dimension, nullptr),
    _max_bond_stress(this->template getParam<Real>("max_bond_stress")),
    _bar_radius(this->template getParam<Real>("rebar_radius")),
    _transitional_slip(this->template getParam<Real>("transitional_slip_value"))
{
  if (_mesh_dimension != this->coupledComponents("displacements"))
    mooseError("In RebarBondSlipConstraint, number of displacements must equal the mesh dimension");

  for (unsigned int i = 0; i < _mesh_dimension; ++i)
  {
    _disp_vars_nums[i] = this->coupled("displacements", i);
    _disp_vars[i] = this->getVar("displacements", i);
  }

  for (std::size_t i = 0; i < _disp_vars_nums.size(); ++i)
    if (_var.number() == _disp_vars_nums[i])
      _component = i;
  if (_component == libMesh::invalid_uint)
    mooseError("Problem with displacements in " + this->_name);

  if (this->isParamValid("output_axial_slip"))
    _output_axial_slip = &(this->writableVariable("output_axial_slip"));
  if (this->isParamValid("output_axial_force"))
    _output_axial_force = &(this->writableVariable("output_axial_force"));
  if (this->isParamValid("output_axial_plastic_slip"))
    _output_axial_plastic_slip = &(this->writableVariable("output_axial_plastic_slip"));
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
    mooseAssert(_bondslip.find(node_id) != _bondslip.end(), "Node not found in bond-slip map");
    if (this->_app.getExecutioner()->lastSolveConverged())
    {
      _bondslip[node_id].slip_min_old = _bondslip[node_id].slip_min;
      _bondslip[node_id].slip_max_old = _bondslip[node_id].slip_max;
      _bondslip[node_id].bond_stress_min_old = _bondslip[node_id].bond_stress_min;
      _bondslip[node_id].bond_stress_max_old = _bondslip[node_id].bond_stress_max;
    }
  }
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
  mooseAssert(_var.currentElem()->dim() == 1, "Elements attached to secondary node must be 1D.");
  for (auto & elem : elems)
  {
    const Elem * elem_ptr = _mesh.elemPtr(elem);
    this->_assembly.reinit(elem_ptr);
    _secondary_node_length += elem_ptr->volume();
    const std::vector<RealGradient> * tangents =
        &_subproblem.assembly(Constraint::_tid, _sys.number()).getFE(FEType(), 1)->get_dxyzdxi();
    const std::vector<Real> * JxW =
        &_subproblem.assembly(Constraint::_tid, _sys.number()).getFE(FEType(), 1)->get_JxW();
    for (std::size_t i = 0; i < tangents->size(); i++)
      _secondary_node_tangent += (*tangents)[i] * (*JxW)[i];
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
    relative_disp(i) = ((_disp_vars[i]->dofValues())[0] - (_disp_vars[i]->slnNeighbor())[0]);

  return relative_disp;
}

template <>
ADRealVectorValue
RebarBondSlipConstraintTempl<true>::computeRelativeDisplacement()
{
  ADRealVectorValue relative_disp;
  for (unsigned int i = 0; i < _mesh_dimension; ++i)
    relative_disp(i) = ((_disp_vars[i]->adDofValues())[0] - (_disp_vars[i]->adSlnNeighbor())[0]);
  return relative_disp;
}

template <bool is_ad>
void
RebarBondSlipConstraintTempl<is_ad>::reinitConstraint()
{
  computeTangent();

  GenericRealVectorValue<is_ad> relative_disp = computeRelativeDisplacement();

  GenericReal<is_ad> slip = relative_disp * _secondary_node_tangent;
  GenericRealVectorValue<is_ad> slip_axial = slip * _secondary_node_tangent;
  GenericRealVectorValue<is_ad> slip_normal = relative_disp - slip_axial;

  const Node * node = _current_node;
  auto it = _bondslip.find(node->id());
  mooseAssert(it != _bondslip.end(), "Node not found in bond-slip map");
  bondSlipData * bond_slip = &(it->second);

  bond_slip->slip_min = std::min(bond_slip->slip_min_old, MetaPhysicL::raw_value(slip));
  bond_slip->slip_max = std::max(bond_slip->slip_max_old, MetaPhysicL::raw_value(slip));

  _bond_stress_deriv = 0.0;
  _bond_stress = 0.0;

  Real plastic_slip = 0;
  switch (_bondslip_model)
  {
    case BondSlipModel::CONCRETE_REBAR_MODEL:
      concreteRebarModel(slip, bond_slip, _bond_stress, _bond_stress_deriv, plastic_slip);
      break;

    case BondSlipModel::ELASTIC_PERFECT_PLASTIC_MODEL:
      elasticPerfectPlasticModel(slip, bond_slip, _bond_stress, _bond_stress_deriv, plastic_slip);
      break;

    default:
      mooseError("Invalid bond slip model");
      break;
  }

  GenericReal<is_ad> bond_force =
      2.0 * libMesh::pi * _bar_radius * _secondary_node_length * _bond_stress;
  GenericReal<is_ad> bond_force_deriv =
      2.0 * libMesh::pi * _bar_radius * _secondary_node_length * _bond_stress_deriv;

  GenericRealVectorValue<is_ad> constraint_force_axial = bond_force * _secondary_node_tangent;
  GenericRealVectorValue<is_ad> constraint_force_normal = this->_penalty * slip_normal;

  _constraint_residual = constraint_force_axial + constraint_force_normal;
  _constraint_jacobian_axial = bond_force_deriv * _secondary_node_tangent;

  bond_slip->bond_stress_min =
      std::min(bond_slip->bond_stress_min_old, MetaPhysicL::raw_value(_bond_stress));
  bond_slip->bond_stress_max =
      std::max(bond_slip->bond_stress_max_old, MetaPhysicL::raw_value(_bond_stress));

  if (_output_axial_slip)
    _output_axial_slip->setNodalValue(MetaPhysicL::raw_value(slip));
  if (_output_axial_force)
    _output_axial_force->setNodalValue(MetaPhysicL::raw_value(_bond_stress));
  if (_output_axial_plastic_slip)
    _output_axial_plastic_slip->setNodalValue(plastic_slip);
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
  mooseError("In ADRebarBondSlipConstraint, computeQpJacobian should not be called. Check "
             "computeQpJacobian implementation.");
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
  mooseError("In ADRebarBondSlipConstraint, computeQpOffDiagJacobian should not be called. Check "
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
    if (var_num == _disp_vars_nums[i])
    {
      coupled_var_is_disp_var = true;
      component = i;
      break;
    }

  return coupled_var_is_disp_var;
}

template <bool is_ad>
void
RebarBondSlipConstraintTempl<is_ad>::concreteRebarModel(const GenericReal<is_ad> slip,
                                                        const bondSlipData * const bond_slip,
                                                        GenericReal<is_ad> & bond_stress,
                                                        GenericReal<is_ad> & bond_stress_deriv,
                                                        Real & plastic_slip) const
{
  const Real slip_min = bond_slip->slip_min;
  const Real slip_max = bond_slip->slip_max;
  using std::abs;
  GenericReal<is_ad> slip_ratio = abs(slip) / _transitional_slip;

  const Real slope = 5.0 * _max_bond_stress / _transitional_slip;
  const Real plastic_slip_max = slip_max - bond_slip->bond_stress_max_old / slope;
  const Real plastic_slip_min = slip_min - bond_slip->bond_stress_min_old / slope;

  plastic_slip = MetaPhysicL::raw_value(_bond_stress) / slope - MetaPhysicL::raw_value(slip);

  bond_stress_deriv = 0.0;
  bond_stress = 0.0;
  if (slip >= slip_max || slip <= slip_min)
  {
    using std::abs;
    if (abs(slip) < _transitional_slip)
    {
      // elastic load or unload
      bond_stress = _max_bond_stress * MathUtils::sign(slip) *
                    (5.0 * slip_ratio - 4.5 * Utility::pow<2>(slip_ratio) +
                     1.4 * Utility::pow<3>(slip_ratio));
      bond_stress_deriv =
          _max_bond_stress *
          (5.0 / _transitional_slip -
           MathUtils::sign(slip) * 4.5 * 2.0 * slip_ratio / _transitional_slip +
           MathUtils::sign(slip) * 1.4 * 3.0 * Utility::pow<2>(slip_ratio) / _transitional_slip);
    }
    else
    {
      // plastic slip
      bond_stress = MathUtils::sign(slip) * _max_bond_stress * 1.9;
      bond_stress_deriv = 0;
    }
  }
  else if (slip > plastic_slip_max && slip < slip_max)
  {
    // unload after positive plastic slip
    bond_stress =
        (slip - plastic_slip_max) * bond_slip->bond_stress_max / (slip_max - plastic_slip_max);
    bond_stress_deriv = slope;
  }
  else if (slip < plastic_slip_min && slip > slip_min)
  {
    // unload after negative plastic slip
    bond_stress =
        (slip - plastic_slip_min) * bond_slip->bond_stress_min / (slip_min - plastic_slip_min);
    bond_stress_deriv = slope;
  }
  else
  {
    // slip at zero stress
    bond_stress = 0;
    bond_stress_deriv = 0;
  }
}

template <bool is_ad>
void
RebarBondSlipConstraintTempl<is_ad>::elasticPerfectPlasticModel(
    const GenericReal<is_ad> slip,
    const bondSlipData * const bond_slip,
    GenericReal<is_ad> & bond_stress,
    GenericReal<is_ad> & bond_stress_deriv,
    Real & plastic_slip) const
{
  const Real slip_min = bond_slip->slip_min;
  const Real slip_max = bond_slip->slip_max;

  const Real slope = _max_bond_stress / _transitional_slip;
  const Real plastic_slip_max = slip_max - bond_slip->bond_stress_max_old / slope;
  const Real plastic_slip_min = slip_min - bond_slip->bond_stress_min_old / slope;

  plastic_slip = MetaPhysicL::raw_value(_bond_stress) / slope - MetaPhysicL::raw_value(slip);

  bond_stress_deriv = 0.0;
  bond_stress = 0.0;
  if (slip >= slip_max || slip <= slip_min)
  {
    using std::abs;
    if (abs(slip) < _transitional_slip)
    {
      // elastic load or unload
      bond_stress = slip * slope;
      bond_stress_deriv = slope;
    }
    else
    {
      // plastic slip
      bond_stress = MathUtils::sign(slip) * _max_bond_stress;
      bond_stress_deriv = 0;
    }
  }
  else if (slip > plastic_slip_max && slip < bond_slip->slip_max)
  {
    // unload after positive plastic slip
    bond_stress = (slip - plastic_slip_max) * bond_slip->bond_stress_max /
                  (bond_slip->slip_max - plastic_slip_max);
    bond_stress_deriv = slope;
  }
  else if (slip < plastic_slip_min && slip > bond_slip->slip_min)
  {
    // unload after negative plastic slip
    bond_stress = (slip - plastic_slip_min) * bond_slip->bond_stress_min /
                  (bond_slip->slip_min - plastic_slip_min);
    bond_stress_deriv = slope;
  }
  else
  {
    // slip at zero stress
    bond_stress = 0;
    bond_stress_deriv = 0;
  }
}

template class RebarBondSlipConstraintTempl<false>;
template class RebarBondSlipConstraintTempl<true>;
