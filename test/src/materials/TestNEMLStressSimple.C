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
#include "TestNEMLStressSimple.h"

registerMooseObject("BlackBearTestApp", TestNEMLStressSimple);

InputParameters
TestNEMLStressSimple::validParams()
{
  InputParameters params = NEMLStressBase::validParams();
  params.addClassDescription("Use a constitutive model in the NEML library "
                             "to compute stress with material properties defined in class.");
  return params;
}

TestNEMLStressSimple::TestNEMLStressSimple(const InputParameters & parameters)
  : NEMLStressBase(parameters)
{
  std::stringstream inputString;

  inputString << "<!-- Units are MPa, mm, s, and Kelvin -->            "
              << "<materials>                                          "
              << "  <!-- Nominally valid over 273 to 1073 K -->        "
              << "  <!-- Source: MCM fit to ASME and collected data -->"
              << "  <le type=\"SmallStrainElasticity\">                "
              << "    <elastic type=\"IsotropicLinearElasticModel\">   "
              << "      <m1_type>youngs</m1_type>                      "
              << "      <m1>100000</m1>                                "
              << "      <m2_type>poissons</m2_type>                    "
              << "      <m2>0.3</m2>                                   "
              << "    </elastic>                                       "
              << "    <alpha>1.0e-4</alpha>                            "
              << "  </le>                                              "
              << "</materials>                                         ";

  std::string mname("le");
  _model = neml::parse_string_unique(inputString.str(), mname);
}

#endif // NEML_ENABLED
