//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralPostprocessor.h"

// Forward Declarations
class PointsSeparation;

template <>
InputParameters validParams<PointsSeparation>();

/**
 * Compute the value of a variable at a specified location.
 *
 * Warning: This postprocessor may result in undefined behavior if utilized with
 * non-continuous elements and the point being located lies on an element boundary.
 */
class PointsSeparation : public GeneralPostprocessor
{
public:
  static InputParameters validParams();

  PointsSeparation(const InputParameters & parameters);

  virtual void initialize() override {}
  virtual void execute() override;
  virtual void finalize() override {}
  virtual Real getValue() override;

protected:
  /// The variable number of the variable we are operating on
  const unsigned int _var_number1;
  const unsigned int _var_number2;
  const unsigned int _var_number3;

  /// A reference to the system containing the variable
  const System & _system1;
  const System & _system2;
  const System & _system3;

  /// The point to locate
  const Point & _point1;
  const Point & _point2;

  /// The value of the variable at the desired location
  Real _value;
};
