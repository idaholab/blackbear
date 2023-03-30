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

class NEMLMaterialPropertyReset : public ElementUserObject
{
public:
  static InputParameters validParams();

  NEMLMaterialPropertyReset(const InputParameters & parameters);
  virtual void initialize();
  virtual void initialSetup();
  virtual void execute();
  virtual void finalize();
  virtual void threadJoin(const UserObject & y);

protected:
  /// Check the temperature and reset history if needed
  virtual void resetQp();

protected:
  /// Coupled temperature
  const VariableValue & _temperature;
  /// Critical temperature value at which to reset
  Real _critical_temperature;
  /// List of NEML model properties to reset
  std::vector<std::string> _props;
  /// NEML material model class to link to
  CauchyStressFromNEML * _neml_material;

private:
  unsigned int _qp;
};
