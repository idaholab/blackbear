//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "AdjointStrainStressGradInnerProduct.h"

registerMooseObject("BlackBearApp", AdjointStrainStressGradInnerProduct);

InputParameters
AdjointStrainStressGradInnerProduct::validParams()
{
  InputParameters params = ElementOptimizationFunctionInnerProduct::validParams();
  params.addRequiredParam<UserObjectName>(
      "stress_grad_name",
      "Name of the stress gradient user object with respect to the material parameter");

  params.addRequiredParam<MaterialPropertyName>(
      "adjoint_strain_name", "Name of the strain property in the adjoint problem");

  params.addClassDescription(
      "Compute the parameter gradient for linear elastic material inversion");
  return params;
}
AdjointStrainStressGradInnerProduct::AdjointStrainStressGradInnerProduct(
    const InputParameters & parameters)
  : ElementOptimizationFunctionInnerProduct(parameters),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _adjoint_strain(getMaterialPropertyByName<RankTwoTensor>(
        getParam<MaterialPropertyName>("adjoint_strain_name"))),
    _stress_grad_uo(getUserObject<BatchStressGrad>("stress_grad_name")),
    _stress_grad_uo_output(_stress_grad_uo.getOutputData())
{
}

Real
AdjointStrainStressGradInnerProduct::computeQpInnerProduct()
{
  if (!_stress_grad_uo.outputReady())
    mooseError("Stress gradient batch material property is not ready to output.");

  const auto index = _stress_grad_uo.getIndex(_current_elem->id());

  return -_adjoint_strain[_qp].doubleContraction(_stress_grad_uo_output[index + _qp]);
}
