//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ElementOptimizationFunctionInnerProduct.h"
#include "BatchStressGrad.h"

class AdjointStrainStressGradInnerProduct : public ElementOptimizationFunctionInnerProduct
{
public:
  static InputParameters validParams();

  AdjointStrainStressGradInnerProduct(const InputParameters & parameters);

protected:
  virtual Real computeQpInnerProduct() override;
  /// Base name of the material system
  const std::string _base_name;
  /// Holds adjoint strain at current quadrature points
  const MaterialProperty<RankTwoTensor> & _adjoint_strain;
  /// UO that holds gradient of stress wrt material parameter at current quadrature points
  const BatchStressGrad & _stress_grad_uo;
  const BatchStressGrad::OutputVector & _stress_grad_uo_output;
};
