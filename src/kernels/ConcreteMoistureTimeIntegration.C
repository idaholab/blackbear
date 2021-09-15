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

#include "ConcreteMoistureTimeIntegration.h"
#include "Material.h"

registerMooseObject("BlackBearApp", ConcreteMoistureTimeIntegration);

InputParameters
ConcreteMoistureTimeIntegration::validParams()
{
  InputParameters params = TimeDerivative::validParams();
  params.addClassDescription("Time derivative term for moisture transport in concrete.");
  return params;
}

ConcreteMoistureTimeIntegration::ConcreteMoistureTimeIntegration(const InputParameters & parameters)
  : TimeDerivative(parameters), _moisture_capacity(nullptr)
{
  if (hasMaterialProperty<Real>("moisture_capacity"))
    _moisture_capacity = &getMaterialProperty<Real>("moisture_capacity");
}

Real
ConcreteMoistureTimeIntegration::computeQpResidual()
{
  // self accumulation term
  if (_moisture_capacity)
    return (*_moisture_capacity)[_qp] * TimeDerivative::computeQpResidual();
  else
    return TimeDerivative::computeQpResidual();
}

Real
ConcreteMoistureTimeIntegration::computeQpJacobian()
{
  if (_moisture_capacity)
    return (*_moisture_capacity)[_qp] * TimeDerivative::computeQpJacobian();
  else
    return TimeDerivative::computeQpJacobian();
}

Real
ConcreteMoistureTimeIntegration::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
