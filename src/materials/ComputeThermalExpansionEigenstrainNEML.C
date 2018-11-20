#include "ComputeThermalExpansionEigenstrainNEML.h"
#include <string>

registerMooseObject("BlackBearApp", ComputeThermalExpansionEigenstrainNEML);

template <>
InputParameters
validParams<ComputeThermalExpansionEigenstrainNEML>()
{
  InputParameters params = validParams<ComputeThermalExpansionEigenstrainBase>();
  params.addRequiredParam<FileName>("database", "Path to NEML XML database.");
  params.addRequiredParam<std::string>("model", "Model name in NEML database.");
  return params;
}

ComputeThermalExpansionEigenstrainNEML::ComputeThermalExpansionEigenstrainNEML(
    const InputParameters & parameters) :
    ComputeThermalExpansionEigenstrainBase(parameters),
    _fname(getParam<FileName>("database")),
    _mname(getParam<std::string>("model")),
    _tstrain(declareProperty<Real>(_base_name + "tstrain")),
    _tstrain_old(getMaterialPropertyOld<Real>(_base_name + "tstrain")),
    _temperature_old(coupledValueOld("temperature"))
{
  // I strongly hesitate to put this here, may change later
  _model = neml::parse_xml_unique(_fname, _mname);
}

void
ComputeThermalExpansionEigenstrainNEML::computeThermalStrain(
    Real & thermal_strain,
    Real & instantaneous_cte)
{
  double nemlCTE = _model->alpha(_temperature[_qp]);
  double nemlCTE_old = _model->alpha(_temperature_old[_qp]);

  thermal_strain = _tstrain_old[_qp] + (nemlCTE + nemlCTE_old) / 2 * (
      _temperature[_qp] - _temperature_old[_qp]);
  
  instantaneous_cte = nemlCTE;
  _tstrain[_qp] = thermal_strain;
}

void ComputeThermalExpansionEigenstrainNEML::initQpStatefulProperties()
{
  ComputeThermalExpansionEigenstrainBase::initQpStatefulProperties();
  _tstrain[_qp] = 0.0;
}
