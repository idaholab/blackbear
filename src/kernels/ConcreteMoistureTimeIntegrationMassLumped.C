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

#include "ConcreteMoistureTimeIntegrationMassLumped.h"

registerMooseObject("BlackBearApp", ConcreteMoistureTimeIntegrationMassLumped);

InputParameters
ConcreteMoistureTimeIntegrationMassLumped::validParams()
{
  InputParameters params = MassLumpedTimeDerivative::validParams();
  params.addClassDescription(
      "Time derivative term with mass lumping for moisture transport in concrete.");
  return params;
}

ConcreteMoistureTimeIntegrationMassLumped::ConcreteMoistureTimeIntegrationMassLumped(
    const InputParameters & parameters)
  : MassLumpedTimeDerivative(parameters), _moisture_capacity(nullptr)
{
  if (hasMaterialProperty<Real>("moisture_capacity"))
    _moisture_capacity = &getMaterialProperty<Real>("moisture_capacity");
}

Real
ConcreteMoistureTimeIntegrationMassLumped::computeQpResidual()
{
  if (_moisture_capacity)
    return (*_moisture_capacity)[_qp] * MassLumpedTimeDerivative::computeQpResidual();
  else
    return MassLumpedTimeDerivative::computeQpResidual();
}

Real
ConcreteMoistureTimeIntegrationMassLumped::computeQpJacobian()
{
  if (_moisture_capacity)
    return (*_moisture_capacity)[_qp] * MassLumpedTimeDerivative::computeQpJacobian();
  else
    return MassLumpedTimeDerivative::computeQpJacobian();
}
