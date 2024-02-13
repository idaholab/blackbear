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

#ifdef NEML_ENABLED

#include "NEMLStateAux.h"

registerMooseObject("BlackBearApp", NEMLStateAux);

InputParameters
NEMLStateAux::validParams()
{
  InputParameters params = AuxKernel::validParams();

  params.addRequiredParam<FileName>("database", "Path to NEML XML database.");
  params.addRequiredParam<std::string>("model", "Model name in NEML database.");
  params.addRequiredParam<std::string>("state_variable",
                                       "Name of the NEML state variable to extract.");
  params.addParam<MaterialPropertyName>(
      "state_vector", "hist", "Material property storing NEML state.");

  return params;
}

NEMLStateAux::NEMLStateAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _file_name(getParam<FileName>("database")),
    _model_name(getParam<std::string>("model")),
    _hist(getMaterialProperty<std::vector<Real>>("state_vector")),
    _var_name(getParam<std::string>("state_variable"))
{
  // We're letting NEML write to raw pointers. Best make sure the stored types are
  // the same on both ends.
  static_assert(std::is_same<Real, double>::value,
                "MOOSE/libMesh must be compiled with double precision Real types");

  // Check if the file exists and can be read
  MooseUtils::checkFileReadable(_file_name);

  // Load model, will throw an exception if it doesn't succeed
  _model = neml::parse_xml_unique(_file_name, _model_name);

  // Get the list of names from neml
  auto names = _model->report_internal_variable_names();

  // Try to find the provided state_variable
  auto loc = std::find(names.begin(), names.end(), _var_name);

  // Check that it was in there
  if (loc == names.end())
    mooseError("The requested state variable was not an output of the "
               "provided NEML model");

  // Store the offset
  _offset = loc - names.begin();
}

Real
NEMLStateAux::computeValue()
{
  // Check that the vector we got has the right size for the model
  mooseAssert(_model->nstore() == _hist[_qp].size(), "History data storage size mismatch");

  // Trivial as we've done all the work in the constructor
  return _hist[_qp][_offset];
}

#endif
