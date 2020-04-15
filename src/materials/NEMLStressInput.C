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
  FileName fname(getParam<FileName>("database"));
  std::ifstream inputStream(fname.c_str());
  if (!inputStream)
    mooseError("Can't open ", fname);

  std::vector<std::string> nemlNames;
  std::vector<Real> nemlValues;
  if (isParamValid("neml_variable_iname"))
  {
    nemlNames = getParam<std::vector<std::string>>("neml_variable_iname");
    nemlValues = getParam<std::vector<Real>>("neml_variable_value");
  }

  std::vector<std::string> xmlVariableNames;
  std::string line, xmlStringForNeml;
  while (getline(inputStream, line))
  {
    if (line.find("material") == std::string::npos)
    {
      xmlStringForNeml += line;
      size_t open = line.find("{");
      size_t close = line.find("}");
      if (open != std::string::npos)
        xmlVariableNames.push_back(line.substr(open + 1, close - (open + 1)));
    }
  }

  errorCheckVariableNames(nemlNames, xmlVariableNames);

  for (auto i = 0; i < nemlNames.size(); ++i)
  {
    std::string varName = nemlNames[i];
    Real varValue = nemlValues[i];
    size_t posOfFirstCurlyBrace = xmlStringForNeml.find(varName) - 1;
    size_t varLenghtWithBothCurlyBraces = varName.length() + 2;
    xmlStringForNeml.replace(
        posOfFirstCurlyBrace, varLenghtWithBothCurlyBraces, std::to_string(varValue));
  }

  _model = neml::parse_string_unique(xmlStringForNeml);
}

void
NEMLStressInput::errorCheckVariableNames(const std::vector<std::string> & nemlNames,
                                         const std::vector<std::string> & xmlNames) const
{
  std::string extraNemlNames = compareVectorsOfStrings(nemlNames, xmlNames);
  std::string extraXmlNames = compareVectorsOfStrings(xmlNames, nemlNames);
  if (!extraNemlNames.empty() || !extraXmlNames.empty())
  {
    std::stringstream msg;
    msg << "Mismatched NEML variable names between xml and BlackBear input file.\n"
        << "  BlackBear variable names not found in xml file: " << extraNemlNames
        << "\n  NEML xml variable names not found in BlackBear input file: " << extraXmlNames;
    mooseError(msg.str());
  }
}

std::string
NEMLStressInput::compareVectorsOfStrings(const std::vector<std::string> & strList1,
                                         const std::vector<std::string> & strList2) const
{
  std::string missingNames;
  for (auto & str1 : strList1)
  {
    bool found = false;
    for (auto & str2 : strList2)
    {
      if (str1 == str2)
      {
        found = true;
        break;
      }
    }
    if (!found)
      missingNames += str1 + " ";
  }
  return missingNames;
}

#endif // NEML_ENABLED
