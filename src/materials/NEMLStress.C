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
#include "NEMLStress.h"

registerMooseObject("BlackBearApp", NEMLStress);

InputParameters
NEMLStress::validParams()
{
  InputParameters params = NEMLStressBase::validParams();
  params.addRequiredParam<FileName>("database", "Path to NEML XML database.");
  params.addRequiredParam<std::string>("model", "Model name in NEML database.");
  return params;
}

NEMLStress::NEMLStress(const InputParameters & parameters)
  : NEMLStressBase(parameters),
    _fname(getParam<FileName>("database")),
    _mname(getParam<std::string>("model"))
{
  _model = neml::parse_xml_unique(_fname, _mname);
}

#endif // NEML_ENABLED
