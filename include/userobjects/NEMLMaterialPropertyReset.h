//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ElementUserObject.h"
#include "CauchyStressFromNEML.h"

/// Reset a NEML state variable when a coupled variable hits a critical value
class NEMLMaterialPropertyReset : public ElementUserObject
{
public:
  static InputParameters validParams();

  NEMLMaterialPropertyReset(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void initialSetup() override;
  virtual void execute() override;
  virtual void finalize() override;
  virtual void threadJoin(const UserObject & y) override;

protected:
  /// Check the temperature and reset history if needed
  virtual void resetQp();

protected:
  /// Coupled variable
  const VariableValue & _variable;
  /// Critical value at which to reset
  Real _critical_value;
  /// List of NEML model properties to reset
  std::vector<std::string> _props;
  /// NEML material model class to link to
  CauchyStressFromNEML * _neml_material;
  /// Cached list of indices
  std::vector<unsigned int> _indices;

private:
  unsigned int _qp;
};
