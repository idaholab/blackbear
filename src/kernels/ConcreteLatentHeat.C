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

#include "ConcreteLatentHeat.h"
#include "Material.h"

registerMooseObject("BlackBearApp", ConcreteLatentHeat);

InputParameters
ConcreteLatentHeat::validParams()
{
  InputParameters params = TimeDerivative::validParams();
  params.addCoupledVar("H", "Relative humididty");
  params.addClassDescription("Latent heat term for heat transfer in concrete.");
  return params;
}

ConcreteLatentHeat::ConcreteLatentHeat(const InputParameters & parameters)
  : TimeDerivative(parameters),
    _ca(getMaterialProperty<Real>("heat_absorption_of_water")),
    _moisture_capacity(nullptr),
    _H_dot(coupledDot("H"))
{
  if (hasMaterialProperty<Real>("moisture_capacity"))
    _moisture_capacity = &getMaterialProperty<Real>("moisture_capacity");
}

Real
ConcreteLatentHeat::computeQpResidual()
{
  // self accumulation term
  if (_moisture_capacity)
    return -_ca[_qp] * (*_moisture_capacity)[_qp] * _H_dot[_qp] * _test[_i][_qp];
  else
    return -_ca[_qp] * _H_dot[_qp] * _test[_i][_qp];
}

Real
ConcreteLatentHeat::computeQpJacobian()
{
  return 0.0;
}

Real
ConcreteLatentHeat::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
