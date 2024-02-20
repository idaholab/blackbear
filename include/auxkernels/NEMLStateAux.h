//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "AuxKernel.h"

#include "neml_interface.h"

/// Pull a NEML internal state variable into an AuxVariable
class NEMLStateAux : public AuxKernel
{
public:
  static InputParameters validParams();

  NEMLStateAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

protected:
  /// File name for the NEML model
  FileName _fname;
  /// Name of the NEML model in the file
  std::string _mname;
  /// A reference to the NEML flat history vector
  const MaterialProperty<std::vector<Real>> & _neml_history;
  /// Name of the NEML state variable to pull
  std::string _var_name;

  /// Stored pointer to the NEML model
  std::unique_ptr<neml::NEMLModel> _model;

  /// Stored offset of the variable into the flat NEML state
  size_t _offset;
};
