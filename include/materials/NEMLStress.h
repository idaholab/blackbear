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

#include <string>
#include <vector>
#include <set>

/**
 * NEMLStress computes the stress using a constitutive model provided by the NEML library.
 * Parameters for that model are defined in an XML file and optionally in the input file.
 */
class NEMLStress : public NEMLStressBase
{
public:
  static InputParameters validParams();
  NEMLStress(const InputParameters & parameters);

protected:
  /// list of {variable} names for substitution in the XML file
  std::vector<std::string> _neml_variable_iname;

  /// Vector of values to substitute in
  std::vector<Real> _neml_variable_value;

  /// Number of {variables{ to substitute in the XML}}
  const std::size_t _nvars;

  /// set of variables in the XML file
  std::set<std::string> _xml_vars;

  /// max number of numbered value parameters (neml_variable_value0, neml_variable_value1, ...)
  static const std::size_t _nvars_max = 8;

  /// NEML XML Input
  std::string _xml;

  /// Find strings in set 1 that are not found in set 2
  std::vector<std::string> setDifference(const std::set<std::string> & set1,
                                         const std::set<std::string> & set2) const;

  /// Build a set of variable names specified in the xml file
  void findXMLVariables();

  /// Check that all variable names in xml file are also in the input file and vice versa
  void errorCheckXMLVariables() const;

  /// Substitute values in for the variables names found in the string version of the xml file
  void replaceXMLVariables();
};
