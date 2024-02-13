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

class NEMLStateAux : public AuxKernel
{
public:
  static InputParameters validParams();

  NEMLStateAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

protected:
  /// The file name for the NEML XML file
  FileName _file_name;
  /// The string model name for the NEML model
  std::string _model_name;
  /// The state vector for the NEML model
  const MaterialProperty<std::vector<Real>> & _hist;
  /// The name of the internal variable in NEML
  std::string _var_name;

  /// Pointer to NEML model
  std::unique_ptr<neml::NEMLModel> _model;
  /// Offset into the NEML history vector for this state
  size_t _offset;
};
