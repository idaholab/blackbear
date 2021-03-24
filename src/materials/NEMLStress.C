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

#include "NEMLStress.h"

registerMooseObject("BlackBearApp", NEMLStress);

InputParameters
NEMLStress::validParams()
{
  InputParameters params = NEMLStressBase::validParams();
  params.addClassDescription("Use a constitutive model in the NEML library to compute stress");
  params.addRequiredParam<FileName>("database", "Path to NEML XML database.");
  params.addRequiredParam<std::string>("model", "Model name in NEML database.");
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
  params.addParam<Real>("neml_variable_value5",
                        "NEML xml variable value for neml_variable_iname[5]");
  params.addParam<Real>("neml_variable_value6",
                        "NEML xml variable value for neml_variable_iname[6]");
  params.addParam<Real>("neml_variable_value7",
                        "NEML xml variable value for neml_variable_iname[7]");

  return params;
}

NEMLStress::NEMLStress(const InputParameters & parameters) : NEMLStressBase(parameters)
{
  FileName fname(getParam<FileName>("database"));
  std::string mname(getParam<std::string>("model"));

  std::string xmlStringForNeml = loadFileIntoString(fname);
  if (isParamValid("neml_variable_iname"))
    replaceXmlVariables(xmlStringForNeml);

  _model = neml::parse_string_unique(xmlStringForNeml, mname);
}

std::string
NEMLStress::compareVectorsOfStrings(const std::vector<std::string> & strList1,
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

std::string
NEMLStress::loadFileIntoString(const FileName & fname) const
{
  // check if the file exists and can be read
  MooseUtils::checkFileReadable(fname);

  // laod file into string
  std::ifstream t(fname.c_str());
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  return str;
}

std::vector<std::string>
NEMLStress::listOfVariablesInXml(const std::string & xmlStringForNeml) const
{
  std::vector<std::string> xmlVariableNames;
  size_t open = xmlStringForNeml.find("{");
  size_t close = xmlStringForNeml.find("}");
  while (open != std::string::npos)
  {
    xmlVariableNames.push_back(xmlStringForNeml.substr(open + 1, close - (open + 1)));
    open = xmlStringForNeml.find("{", open + 1);
    close = xmlStringForNeml.find("}", open);
  }
  return xmlVariableNames;
}

void
NEMLStress::errorCheckVariableNamesFromInputFile(const std::vector<std::string> & nemlNames,
                                                 const std::string & xmlStringForNeml) const
{
  std::vector<std::string> xmlNames = listOfVariablesInXml(xmlStringForNeml);
  std::string extraNemlNames = compareVectorsOfStrings(nemlNames, xmlNames);
  std::string extraXmlNames = compareVectorsOfStrings(xmlNames, nemlNames);
  if (!extraNemlNames.empty() || !extraXmlNames.empty())
  {
    mooseError(
        std::string("Mismatched NEML variable names between xml and BlackBear input file.\n") +
        "  BlackBear input file variable names not found in NEML xml file: " + extraNemlNames +
        "\n  NEML xml file variable names not found in BlackBear input file: " + extraXmlNames);
  }
}

void
NEMLStress::replaceXmlVariables(std::string & xmlStringForNeml) const
{
  std::vector<std::string> nemlNames = getParam<std::vector<std::string>>("neml_variable_iname");
  std::vector<Real> nemlValues = constructNemlSubstitutionList(nemlNames);
  errorCheckVariableNamesFromInputFile(nemlNames, xmlStringForNeml);

  for (size_t i = 0; i < nemlNames.size(); ++i)
  {
    std::string varName = nemlNames[i];
    Real varValue = nemlValues[i];
    size_t posOfFirstCurlyBrace = xmlStringForNeml.find(varName) - 1;
    size_t varLengthWithBothCurlyBraces = varName.length() + 2;
    xmlStringForNeml.replace(
        posOfFirstCurlyBrace, varLengthWithBothCurlyBraces, std::to_string(varValue));
  }
}

std::vector<Real>
NEMLStress::constructNemlSubstitutionList(const std::vector<std::string> & nemlNames) const
{
  std::vector<Real> nemlValues;
  if (isParamValid("neml_variable_iname"))
  {
    if (nemlNames.size() > 8)
      mooseError("NEMLStressVariableInput can only have up to eight neml_variable_iname[0-7]");

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
