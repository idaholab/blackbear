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

class Function;

/**
 * This class uses a function to provide arrayNodal variables with initial conditions.
 */

class ArrayFunctionIndexIC : public ArrayInitialCondition
{
public:
  static InputParameters validParams();

  ArrayFunctionIndexIC(const InputParameters & parameters);

protected:
  virtual RealEigenVector value(const Point & p) override;

  const Function & _function;
};
