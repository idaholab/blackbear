#include "ConcreteMoistureTimeIntegration.h"
#include "Material.h"

template<>
InputParameters validParams<ConcreteMoistureTimeIntegration>()
{
  InputParameters params = validParams<TimeDerivative>();
  return params;
}

ConcreteMoistureTimeIntegration::ConcreteMoistureTimeIntegration(const std::string & name, InputParameters parameters) :
    TimeDerivative(name, parameters),
    _moisture_capacity(getMaterialProperty<Real>("moisture_capacity"))
{
}

Real
ConcreteMoistureTimeIntegration::computeQpResidual()
{
  Real _re = _moisture_capacity[_qp] * TimeDerivative::computeQpResidual(); //self accumulation term

  return _re;

}

Real
ConcreteMoistureTimeIntegration::computeQpJacobian()
{
  return _moisture_capacity[_qp]*TimeDerivative::computeQpJacobian();
}

Real ConcreteMoistureTimeIntegration::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
