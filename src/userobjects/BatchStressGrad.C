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
      "elasticity_tensor_derivative",
      "Name of the elasticity tensor derivative material property.");
  params.addClassDescription("Compute the double contraction of the elasticity tensor derivative "
                             "and the forward mechanical strain");
  return params;
}

BatchStressGrad::BatchStressGrad(const InputParameters & params)
  : BatchStressGradParent(params,
                          // here we pass the derivative of elasticity tensor wrt to the parameter
                          "elasticity_tensor_derivative",
                          // here we pass in the forward strain
                          "forward_mechanical_strain")
{
}

/*
  Note: The following calculation is currently specialized for a linear elastic inverse optimization
  problem. This will be swapped out later on when we can get the gradient of stress w.r.t. the
  interested parameter from NEML for general material models.
*/
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
