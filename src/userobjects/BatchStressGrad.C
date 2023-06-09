//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "BatchStressGrad.h"
#include "libmesh/int_range.h"

registerMooseObject("BlackBearApp", BatchStressGrad);

InputParameters
BatchStressGrad::validParams()
{
  auto params = BatchStressGradParent::validParams();
  params.addRequiredParam<MaterialPropertyName>(
      "elastic_tensor_derivative", "Name of the elastic tensor derivative material property.");
  return params;
}

BatchStressGrad::BatchStressGrad(const InputParameters & params)
  : BatchStressGradParent(params,
                          // here we pass the derivative of elastic tensor wrt to the parameter
                          "elastic_tensor_derivative",
                          // here we pass in the forward strain
                          "forward_mechanical_strain")
{
}

void
BatchStressGrad::batchCompute()
{
  for (const auto i : index_range(_input_data))
  {
    const auto & input = _input_data[i];
    auto & output = _output_data[i];

    const auto & elasticity_dev = std::get<0>(input);
    const auto & strain = std::get<1>(input);

    output = elasticity_dev * strain;
  }
}
