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

class NEMLStateAux : public AuxKernel
{
public:
  static InputParameters validParams();

  NEMLStateAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

protected:
  FileName _fname;
  std::string _mname;
  const MaterialProperty<std::vector<Real>> & _neml_history;
  std::string _var_name;

  std::unique_ptr<neml::NEMLModel> _model;

  size_t _offset;
};
