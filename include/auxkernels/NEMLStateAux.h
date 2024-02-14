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

  /// @brief  Setup from parameters
  NEMLStateAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

protected:
  /// @brief File name for the NEML model
  FileName _fname;
  /// @brief Name of the NEML model in the file
  std::string _mname;
  /// @brief A reference to the NEML flat history vector
  const MaterialProperty<std::vector<Real>> & _neml_history;
  /// @brief Name of the NEML state variable to pull
  std::string _var_name;

  /// @brief Stored pointer to the NEML model
  std::unique_ptr<neml::NEMLModel> _model;

  /// @brief Stored offset of the variable into the flat NEML state
  size_t _offset;
};
