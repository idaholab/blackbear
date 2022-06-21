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

#include <fstream>
#include <streambuf>
#include <algorithm>

#include "NEMLStress.h"
#include "Conversion.h"

registerMooseObject("BlackBearApp", NEMLStress);

InputParameters
NEMLStress::validParams()
{
  InputParameters params = NEMLStressBase::validParams();
  params.addClassDescription("Use a constitutive model in the NEML library to compute stress");
  params.addRequiredParam<FileName>("database", "Path to NEML XML database.");
  params.addRequiredParam<std::string>("model", "Model name in NEML database.");
  params.addParam<std::vector<std::string>>("neml_variable_iname",
                                            "Names of NEML XML name/value pairs");
  params.addParam<std::vector<Real>>("neml_variable_value",
                                     "Corresponding NEML XML variable values");
  for (size_t i = 0; i < _nvars_max; ++i)
  {
    auto istr = Moose::stringify(i);
    params.addDeprecatedParam<Real>(
        "neml_variable_value" + istr,
        "NEML XML variable value for neml_variable_iname[" + istr + "]",
        "Deprecated in favor of the 'neml_variable_value' vector of names parameter");
  }
  return params;
}

NEMLStress::NEMLStress(const InputParameters & parameters)
  : NEMLStressBase(parameters),
    _neml_variable_iname(getParam<std::vector<std::string>>("neml_variable_iname")),
    _neml_variable_value(getParam<std::vector<Real>>("neml_variable_value")),
    _nvars(_neml_variable_iname.size())
{
  // fetch and check inames and values for {variable} replacement
  bool value_vector = false;
  if (!_neml_variable_value.empty())
  {
    if (_nvars != _neml_variable_value.size())
      paramError("neml_variable_value",
                 "If any values are specified the number must match the number of entries in "
                 "neml_variable_iname");
    value_vector = true;
  }
  else if (_nvars > _nvars_max)
    paramError("neml_variable_iname",
               "NEMLStressVariableInput can only have up to ",
               _nvars_max,
               " neml_variable_iname entries");

  // We permit the numbered parameters to override the values vector.
  _neml_variable_value.resize(_nvars);
  for (std::size_t i = 0; i < _nvars; ++i)
  {
    std::string iname = "neml_variable_value" + std::to_string(i);
    if (isParamValid(iname))
      _neml_variable_value[i] = getParam<Real>(iname);
    else if (!value_vector)
      mooseError("Expected parameter '", iname, "' but it was not provided.");
  }

  // check if the file exists and can be read
  const auto fname = getParam<FileName>("database");
  MooseUtils::checkFileReadable(fname);

  // load file into string
  std::ifstream t(fname.c_str());
  _xml.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

  // replace {variables} in the XML file (the _nvars > 0 is an ugly hack)
  if (_nvars > 0)
  {
    findXMLVariables();
    errorCheckXMLVariables();
    replaceXMLVariables();
  }

  // build NEML model object
  auto mname = getParam<std::string>("model");
  _model = neml::parse_string_unique(_xml, mname);
  if (_model->is_damage_model())
    _damage_index = &declareProperty<Real>(_base_name + "damage_index");
}

std::vector<std::string>
NEMLStress::setDifference(const std::set<std::string> & set1,
                          const std::set<std::string> & set2) const
{
  std::vector<std::string> result(set1.size() + set2.size());
  auto it = std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), result.begin());
  return std::vector<std::string>(result.begin(), it);
}

void
NEMLStress::findXMLVariables()
{
  auto open = _xml.find("{");
  auto close = _xml.find("}", open);
  while (open != std::string::npos)
  {
    _xml_vars.insert(_xml.substr(open + 1, close - (open + 1)));
    open = _xml.find("{", open + 1);
    close = _xml.find("}", open);
  }
}

void
NEMLStress::errorCheckXMLVariables() const
{
  std::set<std::string> inames(_neml_variable_iname.begin(), _neml_variable_iname.end());
  if (inames.size() != _nvars)
    paramError("neml_variable_iname", "Duplicate variable names");

  auto d1 = setDifference(inames, _xml_vars);
  auto d2 = setDifference(_xml_vars, inames);

  if (!d1.empty())
    paramError("neml_variable_iname",
               "Input file variable names not found in NEML xml file: ",
               Moose::stringify(d1));
  if (!d2.empty())
    paramError("database",
               "NEML xml file variable names not found in BlackBear input file: ",
               Moose::stringify(d2));
}

void
NEMLStress::replaceXMLVariables()
{
  for (std::size_t i = 0; i < _nvars; ++i)
  {
    auto key = "{" + _neml_variable_iname[i] + "}";
    auto len = key.length();
    auto value = Moose::stringify(_neml_variable_value[i]);

    // substitute all occurrences
    while (true)
    {
      auto pos = _xml.find(key);
      if (pos == std::string::npos)
        break;
      _xml.replace(pos, len, value);
    }
  }
}

#endif // NEML_ENABLED
