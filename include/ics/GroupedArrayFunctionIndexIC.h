//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ArrayInitialCondition.h"
#include "ClusterGroupingLayout.h"

class Function;

/**
 * Initializes a grouped cluster-dynamics array variable from a function of the
 * physical cluster size index (0-based). Explicit sizes are stored directly and
 * grouped bins are projected into the L0/L1 representation.
 */
class GroupedArrayFunctionIndexIC : public ArrayInitialCondition
{
public:
  static InputParameters validParams();

  GroupedArrayFunctionIndexIC(const InputParameters & parameters);

protected:
  virtual RealEigenVector value(const Point & p) override;

  const Function & _function;
  const ClusterGroupingLayout _layout;
};
