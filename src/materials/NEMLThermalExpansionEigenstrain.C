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

#include "NEMLThermalExpansionEigenstrain.h"
#include "CastDualNumber.h"
#include <string>

registerMooseObject("BlackBearApp", NEMLThermalExpansionEigenstrain);

InputParameters
NEMLThermalExpansionEigenstrain::validParams()
{
  InputParameters params = ComputeThermalExpansionEigenstrainBase::validParams();
  params.addClassDescription("Use a model in the NEML library to compute thermal expansion");
  params.addRequiredParam<FileName>("database", "Path to NEML XML database.");
  params.addRequiredParam<std::string>("model", "Model name in NEML database.");
  return params;
}

NEMLThermalExpansionEigenstrain::NEMLThermalExpansionEigenstrain(const InputParameters & parameters)
  : ComputeThermalExpansionEigenstrainBase(parameters),
    _fname(getParam<FileName>("database")),
    _mname(getParam<std::string>("model")),
    _thermal_strain(declareGenericProperty<Real, false>(_base_name + "tstrain")),
    _thermal_strain_old(getMaterialPropertyOld<Real>(_base_name + "tstrain")),
    _temperature_old(coupledValueOld("temperature"))
{
  _model = neml::parse_xml_unique(_fname, _mname);
}

ValueAndDerivative<false>
NEMLThermalExpansionEigenstrain::computeThermalStrain()
{
  Real nemlCTE = _model->alpha(MetaPhysicL::raw_value(_temperature[_qp]));
  Real nemlCTE_old = _model->alpha(_temperature_old[_qp]);

  const auto thermal_strain =
      _thermal_strain_old[_qp] +
      (nemlCTE + nemlCTE_old) / 2 * (_temperature[_qp] - _temperature_old[_qp]);
  _thermal_strain[_qp] = dual_number_cast<GenericReal<false>>(thermal_strain);

  return thermal_strain;
}

void
NEMLThermalExpansionEigenstrain::initQpStatefulProperties()
{
  ComputeThermalExpansionEigenstrainBase::initQpStatefulProperties();
  _thermal_strain[_qp] = 0.0;
}

#endif // NEML_ENABLED
