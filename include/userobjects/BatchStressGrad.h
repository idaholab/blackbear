//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "BatchMaterial.h"

typedef BatchMaterial<
    // tuple representation
    BatchMaterialUtils::TupleStd,
    // output data type
    RankTwoTensor,
    // gathered input data types:
    BatchMaterialUtils::GatherMatProp<RankFourTensor>,
    BatchMaterialUtils::GatherMatProp<RankTwoTensor>>

    BatchStressGradParent;

class BatchStressGrad : public BatchStressGradParent
{
public:
  static InputParameters validParams();

  BatchStressGrad(const InputParameters & params);

  void batchCompute() override;
};
