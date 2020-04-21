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
#include "NEMLStressVariableInput.h"

registerMooseObject("BlackBearApp", NEMLStressVariableInput);

InputParameters
NEMLStressVariableInput::validParams()
{
  InputParameters params = NEMLStressBase::validParams();
  params.addClassDescription("Use a constitutive model in the NEML library to compute stress");
  params.addRequiredParam<FileName>("database", "Path to NEML XML database.");
  params.addParam<std::vector<std::string>>("neml_variable_iname",
                                            "Names of NEML xml name/value pairs");

  params.addParam<Real>("neml_variable_value0",
                        "NEML xml variable value for neml_variable_iname[0]");
  params.addParam<Real>("neml_variable_value1",
                        "NEML xml variable value for neml_variable_iname[1]");
  params.addParam<Real>("neml_variable_value2",
                        "NEML xml variable value for neml_variable_iname[2]");
  params.addParam<Real>("neml_variable_value3",
                        "NEML xml variable value for neml_variable_iname[3]");
  params.addParam<Real>("neml_variable_value4",
                        "NEML xml variable value for neml_variable_iname[4]");
  return params;
}

NEMLStressVariableInput::NEMLStressVariableInput(const InputParameters & parameters)
  : NEMLStressBase(parameters)
{
  std::vector<std::string> nemlNames;
  if (isParamValid("neml_variable_iname"))
  {
    nemlNames = getParam<std::vector<std::string>>("neml_variable_iname");
  }

  std::vector<Real> nemlValues = constructNemlSubstitutionList(nemlNames);

  FileName fname(getParam<FileName>("database"));
  std::string xmlStringForNeml = parseAndReplaceXmlVariables(nemlNames, nemlValues, fname);

  _model = neml::parse_string_unique(xmlStringForNeml);
}

std::vector<Real>
NEMLStressVariableInput::constructNemlSubstitutionList(
    const std::vector<std::string> & nemlNames) const
{
  std::vector<Real> nemlValues;
  if (isParamValid("neml_variable_iname"))
  {
    if (nemlNames.size() > 5)
      mooseError("NEMLStressVariableInput can only have up to five neml_variable_iname's.");

    for (size_t i = 0; i < nemlNames.size(); ++i)
    {
      std::string iname = "neml_variable_value" + std::to_string(i);
      if (isParamValid(iname))
        nemlValues.push_back(getParam<Real>(iname));
      else
        mooseError(
            "NEMLStressVariableInput: incorrect name or number of neml_variable_value keywords.");
    }
  }
  return nemlValues;
}

#endif // NEML_ENABLED
