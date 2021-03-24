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

#include "NEMLStressBase.h"
#include "neml_interface.h"

/**
 * NEMLStress computes the stress using a constitutive model provided by the NEML library.
 * Parameters for that model are defined in an xml file and optionally in the input file.
 */

class NEMLStress : public NEMLStressBase
{
public:
  static InputParameters validParams();
  NEMLStress(const InputParameters & parameters);

private:
  /**
   * Get values from the input file that will be substituted in for the nemlNames provided in the
   *input file
   * @param nemlNames variables names from input file
   * @return vector containing the values that will be substituted in for the each nemlNames
   **/
  std::vector<Real> constructNemlSubstitutionList(const std::vector<std::string> & nemlNames) const;

  /**
   * Error checking: find strings in string list 1 that are not found in string list 2
   * @param strList1
   * @param strList2
   * @return return strings from strList1 not found in strList2
   **/
  std::string compareVectorsOfStrings(const std::vector<std::string> & strList1,
                                      const std::vector<std::string> & strList2) const;

  /**
   * Parse the xml file into a string
   * @param fname xml file name to be read in
   * @return string containing file contents
   **/
  std::string loadFileIntoString(const FileName & fname) const;

  /**
   * Get a list of variable names specified in the xml file
   * @param xmlStringForNeml string containing xml file
   * @return list of the variable names found in the xml file.
   **/
  std::vector<std::string> listOfVariablesInXml(const std::string & xmlStringForNeml) const;

  /**
   * Check that all variable names in xml file are also in the input file and vice versa
   * @param nemlNames variables names from input file
   * @param xmlStringForNeml string containing the xml file
   **/
  void errorCheckVariableNamesFromInputFile(const std::vector<std::string> & nemlNames,
                                            const std::string & xmlStringForNeml) const;

  /**
   * Substitute values in for the variables names found in the string version of the xml file
   * @param xmlStringForNeml string containing the xml file, returned version of this string has
   *values substituted in for variables names
   **/

  void replaceXmlVariables(std::string & xmlStringForNeml) const;
};
