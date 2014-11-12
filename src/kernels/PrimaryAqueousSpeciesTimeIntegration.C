#include "PrimaryAqueousSpeciesTimeIntegration.h"
#include "Material.h"

template<>
InputParameters validParams<PrimaryAqueousSpeciesTimeIntegration>()
{
  InputParameters params = validParams<TimeDerivative>();
  params.addParam<std::string>("property_name","porosity","The real material property to use");
//   params.addCoupledVar("mineral_compositions","mineral Aux variable names involved");
//   params.addParam<std::vector<Real> >("sto_v","stochiometric coeff ofreactant species");

  return params;
}

PrimaryAqueousSpeciesTimeIntegration::PrimaryAqueousSpeciesTimeIntegration(const std::string & name, InputParameters parameters) :
    TimeDerivative(name, parameters),
    _porosity(getMaterialProperty<Real>(getParam<std::string>("property_name")))
//    _sto_v(getParam<std::vector<Real> >("sto_v"))
{

//   int n = coupledComponents("mineral_compositions");
//   _dvals_dt.resize(n);

//   for (unsigned int i=0; i<_dvals_dt.size(); ++i)
//   {
//     _dvals_dt[i] = &coupledDot("mineral_compositions", i);
//   }

}

Real
PrimaryAqueousSpeciesTimeIntegration::computeQpResidual()
{
  Real _re = _porosity[_qp] * TimeDerivative::computeQpResidual(); //self accumulation term

//   for (unsigned int i=0; i < _dvals_dt.size(); ++i)
//     _re += _porosity[_qp] * _sto_v[i] * (*_dvals_dt[i])[_qp] * _test[_i][_qp];
  return _re;

}

Real
PrimaryAqueousSpeciesTimeIntegration::computeQpJacobian()
{
  return _porosity[_qp]*TimeDerivative::computeQpJacobian();
}

Real PrimaryAqueousSpeciesTimeIntegration::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
