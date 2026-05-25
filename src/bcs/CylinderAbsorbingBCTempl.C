//* This file is part of BlackBear: https://github.com/idaholab/blackbear
//*
//* All rights reserved, see NOTICE.txt for full restrictions
//* https://github.com/idaholab/blackbear/blob/main/NOTICE.txt
//*
//* Get help at https://mooseframework.inl.gov/blackbear

#include "CylinderAbsorbingBCTempl.h"

registerMooseObject("BlackBearApp", CylinderAbsorbingBC);
registerMooseObject("BlackBearApp", ADCylinderAbsorbingBC);

template <bool is_ad>
InputParameters
CylinderAbsorbingBCTempl<is_ad>::validParams()
{
  InputParameters params = GenericIntegratedBC<is_ad>::validParams();
  params.addClassDescription(
      "Lysmer-Kuhlemeyer absorbing boundary condition for a smooth far-field "
      "surface of a linear isotropic elastic domain. Dynamic only.");
  MooseEnum component("x y z");
  params.addRequiredParam<MooseEnum>(
      "component", component, "The displacement component this BC acts on.");
  params.addRequiredCoupledVar("displacements",
                               "The Cartesian displacement variables, in order x y z.");
  params.addRequiredParam<MaterialPropertyName>("density", "Mass density material property.");
  params.addRequiredParam<MaterialPropertyName>("shear_modulus",
                                                "Shear modulus (mu) material property.");
  params.addRequiredParam<MaterialPropertyName>("lambda",
                                                "First Lame parameter material property.");
  return params;
}

template <bool is_ad>
CylinderAbsorbingBCTempl<is_ad>::CylinderAbsorbingBCTempl(const InputParameters & parameters)
  : GenericIntegratedBC<is_ad>(parameters),
    _component(this->template getParam<MooseEnum>("component")),
    _ndisp(this->coupledComponents("displacements")),
    _disp_dot(_ndisp),
    _density(this->template getMaterialProperty<Real>("density")),
    _shear_modulus(this->template getMaterialProperty<Real>("shear_modulus")),
    _lambda(this->template getMaterialProperty<Real>("lambda"))
{
  if (_component >= static_cast<int>(_ndisp))
    this->paramError("component",
                     "Component index ",
                     _component,
                     " is out of range for ",
                     _ndisp,
                     " coupled displacements.");

  for (const auto i : make_range(_ndisp))
    _disp_dot[i] = &this->template coupledGenericDot<is_ad>("displacements", i);
}

template <bool is_ad>
GenericReal<is_ad>
CylinderAbsorbingBCTempl<is_ad>::computeQpResidual()
{
  GenericRealVectorValue<is_ad> velocity;
  for (const auto i : make_range(_ndisp))
    velocity(i) = (*_disp_dot[i])[_qp];

  const Real rho = _density[_qp];
  const Real mu = _shear_modulus[_qp];
  const Real c_p = std::sqrt((_lambda[_qp] + 2.0 * mu) / rho);
  const Real c_s = std::sqrt(mu / rho);

  const auto & n = this->_normals[_qp];
  const auto n_dot_v = n * velocity;
  const auto n_k = n(_component);
  const auto v_k = velocity(_component);

  return _test[_i][_qp] * (rho * c_p * n_dot_v * n_k + rho * c_s * (v_k - n_dot_v * n_k));
}

InputParameters
CylinderAbsorbingBC::validParams()
{
  return CylinderAbsorbingBCTempl<false>::validParams();
}

CylinderAbsorbingBC::CylinderAbsorbingBC(const InputParameters & parameters)
  : CylinderAbsorbingBCTempl<false>(parameters), _disp_var(_ndisp)
{
  for (const auto i : make_range(_ndisp))
    _disp_var[i] = coupled("displacements", i);
}

Real
CylinderAbsorbingBC::computeQpJacobian()
{
  const Real rho = _density[_qp];
  const Real mu = _shear_modulus[_qp];
  const Real c_p = std::sqrt((_lambda[_qp] + 2.0 * mu) / rho);
  const Real c_s = std::sqrt(mu / rho);

  const Real n_k = _normals[_qp](_component);
  const Real dv_du = _phi[_j][_qp] / _dt;

  return _test[_i][_qp] * (rho * c_p * n_k * n_k + rho * c_s * (1.0 - n_k * n_k)) * dv_du;
}

Real
CylinderAbsorbingBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  unsigned int j_component = libMesh::invalid_uint;
  for (const auto i : make_range(_ndisp))
    if (jvar == _disp_var[i])
    {
      j_component = i;
      break;
    }
  if (j_component == libMesh::invalid_uint)
    return 0.0;

  const Real rho = _density[_qp];
  const Real mu = _shear_modulus[_qp];
  const Real c_p = std::sqrt((_lambda[_qp] + 2.0 * mu) / rho);
  const Real c_s = std::sqrt(mu / rho);

  const Real n_k = _normals[_qp](_component);
  const Real n_j = _normals[_qp](j_component);
  const Real dv_du = _phi[_j][_qp] / _dt;

  return _test[_i][_qp] * rho * (c_p - c_s) * n_j * n_k * dv_du;
}

template class CylinderAbsorbingBCTempl<false>;
template class CylinderAbsorbingBCTempl<true>;
