//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/primary/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

// MOOSE includes
#include "RebarBondSlipConstraint.h"
#include "Assembly.h"
#include "SystemBase.h"
#include "FEProblem.h"
#include "MathUtils.h"

#include "libmesh/string_to_enum.h"

registerMooseObject("BlackBearApp", RebarBondSlipConstraint);

defineLegacyParams(RebarBondSlipConstraint);

InputParameters
RebarBondSlipConstraint::validParams()
{
  InputParameters params = EqualValueEmbeddedConstraint::validParams();
  params.addClassDescription(
      "This is a constraint enforcing the bod-slip behavior between concrete and rebar");
  params.addRequiredParam<unsigned int>("component",
                                        "An integer corresponding to the direction "
                                        "the variable this kernel acts in. (0 for x, "
                                        "1 for y, 2 for z)");
  params.addCoupledVar(
      "displacements",
      "The displacements appropriate for the simulation geometry and coordinate system");
  params.addParam<bool>("debug", false, "whether to print out debug messages");
  params.addParam<Real>("max_bondstress", 0.0, "Maximum bond stress");
  params.addParam<Real>("frictional_bondstress", 0.0, "Bond stress due to friction");

  params.addParam<std::vector<Real>>(
      "transitional_slip_values",
      "Singificant slip values at which the bondstress curve changes pattern/slope or "
      "trnsitions to a different function");
  params.addParam<Real>(
      "ultimate_slip", 0.05, "Ultimate value of slip at which the concrete and rebar debonds");
  params.addParam<Real>("rebar_radius", 1.0, "Radius of the rebar");
  return params;
}

RebarBondSlipConstraint::RebarBondSlipConstraint(const InputParameters & parameters)
  : EqualValueEmbeddedConstraint(parameters),
    _component(getParam<unsigned int>("component")),
    _mesh_dimension(_mesh.dimension()),
    _var_nums(_mesh_dimension, libMesh::invalid_uint),
    _vars(_mesh_dimension, nullptr),
    _debug(getParam<bool>("debug")),
    _max_bondstress(getParam<Real>("max_bondstress")),
    _frictional_bondstress(getParam<Real>("frictional_bondstress")),
    _ultimate_slip(getParam<Real>("ultimate_slip")),
    _bar_radius(getParam<Real>("rebar_radius")),
    _transitional_slip(getParam<std::vector<Real>>("transitional_slip_values"))
{
  if (_mesh_dimension != coupledComponents("displacements"))
    mooseError("In RebarBondSlipConstraint, number of displacements must equal the mesh dimension");

  for (unsigned int i = 0; i < _mesh_dimension; ++i)
  {
    _var_nums[i] = coupled("displacements", i);
    _vars[i] = getVar("displacements", i);
  }
}

void
RebarBondSlipConstraint::initialSetup()
{
  for (auto it = _secondary_to_primary_map.begin(); it != _secondary_to_primary_map.end(); ++it)
    if (_bondslip.find(it->first) == _bondslip.end())
      _bondslip.insert(std::pair<dof_id_type, bondSlipData>(it->first,
                                                            bondSlipData())); // initialize
}

void
RebarBondSlipConstraint::timestepSetup()
{
  for (auto iter = _secondary_to_primary_map.begin(); iter != _secondary_to_primary_map.end();
       ++iter)
  {
    dof_id_type node_id = iter->first;
    auto it = _bondslip.find(node_id);
    mooseAssert(it != _bondslip.end(), "Node not found in bond-slip map");

    _bondslip[node_id].slip_min_old = _bondslip[node_id].slip_min;
    _bondslip[node_id].slip_max_old = _bondslip[node_id].slip_max;
    _bondslip[node_id].bondstress_min_old = _bondslip[node_id].bondstress_min;
    _bondslip[node_id].bondstress_max_old = _bondslip[node_id].bondstress_max;
  }
}

bool
RebarBondSlipConstraint::shouldApply()
{
  if (_debug)
  {
    std::cout << "===========================================\n";
    std::cout << "node id: " << _current_node->id() << std::endl;
    std::cout << "at coord: " << (Point)*_current_node << std::endl;
  }
  auto it = _secondary_to_primary_map.find(_current_node->id());

  if (it != _secondary_to_primary_map.end())
  {
    const Elem * primary_elem = _mesh.elemPtr(it->second);
    std::vector<Point> points = {*_current_node};

    // reinit variables on the primary element at the secondary point
    _fe_problem.setNeighborSubdomainID(primary_elem, 0);
    _fe_problem.reinitNeighborPhys(primary_elem, points, 0);

    reinitConstraint();

    return true;
  }
  return false;
}

void
RebarBondSlipConstraint::computeTangent()
{
  _secondary_tangent = 0.0;

  // get normals
  // get connected elements of the current node
  const std::map<dof_id_type, std::vector<dof_id_type>> & node_to_elem_map = _mesh.nodeToElemMap();
  auto node_to_elem_pair = node_to_elem_map.find(_current_node->id());
  mooseAssert(node_to_elem_pair != node_to_elem_map.end(), "Missing entry in node to elem map");
  const std::vector<dof_id_type> & elems = node_to_elem_pair->second;

  for (auto & elem : elems)
  {
    Elem * elem_ptr = _mesh.elemPtr(elem);
    _assembly.reinit(elem_ptr, 0);
    _current_elem_volume += _assembly.elemVolume();
    // calculate phi and dphi for this element
    FEType fe_type(Utility::string_to_enum<Order>("first"),
                   Utility::string_to_enum<FEFamily>("lagrange"));
    std::unique_ptr<FEBase> fe(FEBase::build(1, fe_type));
    fe->attach_quadrature_rule(const_cast<QBase *>(_assembly.qRule()));
    const std::vector<RealGradient> * tangents = &fe->get_dxyzdxi();
    unsigned side = 0;
    fe->reinit(elem_ptr, side);
    for (unsigned i = 0; i < tangents->size(); i++)
      _secondary_tangent += (*tangents)[i];
  }

  _secondary_tangent /= _secondary_tangent.norm();
  _current_elem_volume /= elems.size();

  if (_debug)
    std::cout << "tangent: " << _secondary_tangent << std::endl;
}

void
RebarBondSlipConstraint::reinitConstraint()
{
  computeTangent();

  // Build up residual vector
  RealVectorValue relative_disp;
  for (unsigned int i = 0; i < _mesh_dimension; ++i)
    relative_disp(i) = ((_vars[i]->dofValues())[0] - (_vars[i]->slnNeighbor())[0]);

  Real slip = relative_disp * _secondary_tangent;
  RealVectorValue slip_axial = slip * _secondary_tangent;
  RealVectorValue slip_normal = relative_disp - slip_axial;
  Real slip_ratio = std::abs(slip) / _transitional_slip[0];

  if (_debug)
    std::cout << "Slip = " << slip << ".\n";

  const Node * node = _current_node;
  auto it = _bondslip.find(node->id());
  mooseAssert(it != _bondslip.end(), "Node not found in bond-slip map");
  bondSlipData bond_slip = it->second;

  bond_slip.slip_min = std::min(bond_slip.slip_min_old, slip);
  bond_slip.slip_max = std::max(bond_slip.slip_max_old, slip);

  if (_debug)
  {
    std::cout << "Slip_min = " << bond_slip.slip_min << ".\n";
    std::cout << "Slip_min_old = " << bond_slip.slip_min_old << ".\n";
    std::cout << "Slip_max = " << bond_slip.slip_max << ".\n";
    std::cout << "Slip_max_old = " << bond_slip.slip_max_old << ".\n";
    std::cout << "Bondstress_min = " << bond_slip.bondstress_min << ".\n";
    std::cout << "Bondstress_min_old = " << bond_slip.bondstress_min_old << ".\n";
    std::cout << "Bondstress_max = " << bond_slip.bondstress_max << ".\n";
    std::cout << "Bondstress_max_old = " << bond_slip.bondstress_max_old << ".\n";
  }

  Real slope = 5.0 * _max_bondstress / _transitional_slip[0];
  Real plastic_slip_max = bond_slip.slip_max - bond_slip.bondstress_max / slope;
  Real plastic_slip_min = bond_slip.slip_min - bond_slip.bondstress_min / slope;

  _bond_stress_deriv = 0.0;

  if (slip >= bond_slip.slip_max || slip <= bond_slip.slip_min)
  {
    if (std::abs(slip) < _transitional_slip[0])
    {
      _bond_stress = _max_bondstress * MathUtils::sign(slip) *
                     (5.0 * slip_ratio - 4.5 * slip_ratio * slip_ratio +
                      1.4 * slip_ratio * slip_ratio * slip_ratio);
      _bond_stress_deriv =
          _max_bondstress * MathUtils::sign(slip) *
          (5.0 / _transitional_slip[0] - 4.5 * 2.0 * slip_ratio / _transitional_slip[0] +
           1.4 * 3.0 * slip_ratio * slip_ratio / _transitional_slip[0]);
    }
    else if (slip >= _transitional_slip[0] && slip < _ultimate_slip)
      _bond_stress = 1.9 * _max_bondstress;
    else if (slip <= -_transitional_slip[0] && slip > -_ultimate_slip)
      _bond_stress = -1.9 * _max_bondstress;
    else
      _bond_stress = _frictional_bondstress * MathUtils::sign(slip);
  }
  else if (slip > plastic_slip_max && slip < bond_slip.slip_max)
  {
    _bond_stress = (slip - plastic_slip_max) * bond_slip.bondstress_max /
                   (bond_slip.slip_max - plastic_slip_max);

    _bond_stress_deriv = bond_slip.bondstress_max / (bond_slip.slip_max - plastic_slip_max);
  }
  else if (slip < plastic_slip_min && slip > bond_slip.slip_min)
  {
    _bond_stress = (slip - plastic_slip_min) * bond_slip.bondstress_min /
                   (bond_slip.slip_min - plastic_slip_min);
    _bond_stress_deriv = bond_slip.bondstress_min / (bond_slip.slip_min - plastic_slip_min);
  }
  else
    _bond_stress = _frictional_bondstress;

  if (_debug)
  {
    std::cout << "Bondstress = " << _bond_stress << "\n";
    std::cout << "Bondstress Derivative = " << _bond_stress_deriv << "\n";
  }

  Real bond_force = 2.0 * libMesh::pi * _bar_radius * _current_elem_volume * _bond_stress;
  Real bond_force_deriv =
      2.0 * libMesh::pi * _bar_radius * _current_elem_volume * _bond_stress_deriv;

  RealVectorValue constraint_force_axial = bond_force * _secondary_tangent;
  RealVectorValue constraint_force_normal = _penalty * slip_normal;

  _constraint_residual = constraint_force_axial + constraint_force_normal;
  _constraint_jacobian_axial = bond_force_deriv * _secondary_tangent;

  bond_slip.bondstress_min = std::min(bond_slip.bondstress_min_old, _bond_stress);
  bond_slip.bondstress_max = std::max(bond_slip.bondstress_max_old, _bond_stress);

  if (_fe_problem.converged())
  {
    _bondslip[node->id()].slip_min = bond_slip.slip_min;
    _bondslip[node->id()].slip_max = bond_slip.slip_max;
    _bondslip[node->id()].bondstress_min = bond_slip.bondstress_min;
    _bondslip[node->id()].bondstress_max = bond_slip.bondstress_max;
  }
}

Real
RebarBondSlipConstraint::computeQpResidual(Moose::ConstraintType type)
{
  Real resid = _constraint_residual(_component);

  switch (type)
  {
    case Moose::Secondary:
      return resid * _test_secondary[_i][_qp];

    case Moose::Primary:
      return -resid * _test_primary[_i][_qp];
  }

  return 0.0;
}

Real
RebarBondSlipConstraint::computeQpJacobian(Moose::ConstraintJacobianType type)
{
  Real jac_axial = _constraint_jacobian_axial(_component);

  switch (type)
  {
    case Moose::SecondarySecondary:
      return _phi_secondary[_j][_qp] * jac_axial * _secondary_tangent(_component) *
                 _test_secondary[_i][_qp] +
             _phi_secondary[_j][_qp] * _penalty * _test_secondary[_i][_qp] *
                 (1.0 - _secondary_tangent(_component) * _secondary_tangent(_component));

    case Moose::SecondaryPrimary:
      return -_phi_primary[_j][_qp] * jac_axial * _secondary_tangent(_component) *
                 _test_secondary[_i][_qp] -
             _phi_primary[_j][_qp] * _penalty * _test_secondary[_i][_qp] *
                 (1.0 - _secondary_tangent(_component) * _secondary_tangent(_component));

    case Moose::PrimarySecondary:
      return -_test_primary[_i][_qp] * jac_axial * _secondary_tangent(_component) *
                 _phi_secondary[_j][_qp] -
             _test_primary[_i][_qp] * _penalty * _phi_secondary[_j][_qp] *
                 (1.0 - _secondary_tangent(_component) * _secondary_tangent(_component));

    case Moose::PrimaryPrimary:
      return _test_primary[_i][_qp] * jac_axial * _secondary_tangent(_component) *
                 _phi_primary[_j][_qp] +
             _test_primary[_i][_qp] * _penalty * _phi_primary[_j][_qp] *
                 (1.0 - _secondary_tangent(_component) * _secondary_tangent(_component));

    default:
      mooseError("Unsupported type");
      break;
  }
  return 0.0;
}

Real
RebarBondSlipConstraint::computeQpOffDiagJacobian(Moose::ConstraintJacobianType type,
                                                  unsigned int jvar)
{
  Real jac_axial = _constraint_jacobian_axial(_component);

  unsigned int coupled_component;
  Real tangent_component_in_coupled_var_dir = 1.0;
  if (getCoupledVarComponent(jvar, coupled_component))
    tangent_component_in_coupled_var_dir = _secondary_tangent(coupled_component);

  switch (type)
  {
    case Moose::SecondarySecondary:
      return _phi_secondary[_j][_qp] * jac_axial * tangent_component_in_coupled_var_dir *
                 _test_secondary[_i][_qp] -
             _phi_secondary[_j][_qp] * _penalty * _test_secondary[_i][_qp] *
                 _secondary_tangent(_component) * tangent_component_in_coupled_var_dir;

    case Moose::SecondaryPrimary:
      return -_phi_primary[_j][_qp] * jac_axial * tangent_component_in_coupled_var_dir *
                 _test_secondary[_i][_qp] +
             _phi_primary[_j][_qp] * _penalty * _test_secondary[_i][_qp] *
                 _secondary_tangent(_component) * tangent_component_in_coupled_var_dir;

    case Moose::PrimarySecondary:
      return -_test_primary[_i][_qp] * jac_axial * tangent_component_in_coupled_var_dir *
                 _phi_secondary[_j][_qp] +
             _test_primary[_i][_qp] * _penalty * _phi_secondary[_j][_qp] *
                 _secondary_tangent(_component) * tangent_component_in_coupled_var_dir;

    case Moose::PrimaryPrimary:
      return _test_primary[_i][_qp] * jac_axial * tangent_component_in_coupled_var_dir *
                 _phi_primary[_j][_qp] -
             _test_primary[_i][_qp] * _penalty * _phi_primary[_j][_qp] *
                 _secondary_tangent(_component) * tangent_component_in_coupled_var_dir;

    default:
      mooseError("Unsupported type");
      break;
  }
  return 0.0;
}

bool
RebarBondSlipConstraint::getCoupledVarComponent(unsigned int var_num, unsigned int & component)
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
