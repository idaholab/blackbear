#include "ConcreteThermalTimeIntegration.h"
#include "Material.h"

template<>
InputParameters validParams<ConcreteThermalTimeIntegration>()
{
  InputParameters params = validParams<TimeDerivative>();
  params.addParam<std::string>("property_name","thermal_capacity","The real material property to use");
  return params;
}

ConcreteThermalTimeIntegration::ConcreteThermalTimeIntegration(const std::string & name, InputParameters parameters) :
    TimeDerivative(name, parameters),
    _thermal_capacity(getMaterialProperty<Real>("thermal_capacity"))
{
}

Real
ConcreteThermalTimeIntegration::computeQpResidual()
{
  Real _re = _thermal_capacity[_qp] * TimeDerivative::computeQpResidual(); //self accumulation term

  return _re;

}

Real
ConcreteThermalTimeIntegration::computeQpJacobian()
{
  return _thermal_capacity[_qp]*TimeDerivative::computeQpJacobian();
}

Real ConcreteThermalTimeIntegration::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
