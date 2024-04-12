/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                       BlackBear                              */
/*                                                              */
/*           (c) 2017 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

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
