/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                        Grizzly                               */
/*                                                              */
/*           (c) 2015 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "ConcreteMoistureTimeIntegration.h"
#include "Material.h"

template<>
InputParameters validParams<ConcreteMoistureTimeIntegration>()
{
  InputParameters params = validParams<TimeDerivative>();
  return params;
}

ConcreteMoistureTimeIntegration::ConcreteMoistureTimeIntegration(const InputParameters & parameters) :
    TimeDerivative(parameters),
    _moisture_capacity(getMaterialProperty<Real>("moisture_capacity"))
{
}

Real
ConcreteMoistureTimeIntegration::computeQpResidual()
{
  return _moisture_capacity[_qp] * TimeDerivative::computeQpResidual(); // self accumulation term
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
