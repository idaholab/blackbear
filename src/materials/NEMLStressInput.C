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
#include "NEMLStressInput.h"

registerMooseObject("BlackBearApp", NEMLStressInput);

InputParameters
NEMLStressInput::validParams()
{
  InputParameters params = NEMLStressBase::validParams();
  params.addClassDescription("Use a constitutive model in the NEML library to compute stress");
  params.addRequiredParam<FileName>("database", "Path to NEML XML database.");
  params.addParam<std::vector<std::string>>("neml_variable_iname",
                                            "Names of NEML xml name/value pairs");
  params.addParam<std::vector<Real>>("neml_variable_value", "Values of NEML xml name/value pairs");
  return params;
}

NEMLStressInput::NEMLStressInput(const InputParameters & parameters) : NEMLStressBase(parameters)
{
  std::vector<std::string> nemlNames;
  std::vector<Real> nemlValues;
  if (isParamValid("neml_variable_iname"))
  {
    nemlNames = getParam<std::vector<std::string>>("neml_variable_iname");
    nemlValues = getParam<std::vector<Real>>("neml_variable_value");
  }

  FileName fname(getParam<FileName>("database"));
  std::string xmlStringForNeml = parseAndReplaceXmlVariables(nemlNames, nemlValues, fname);

  _model = neml::parse_string_unique(xmlStringForNeml);
}

#endif // NEML_ENABLED
