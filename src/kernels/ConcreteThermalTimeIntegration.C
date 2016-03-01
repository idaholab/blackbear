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

#include "ConcreteThermalTimeIntegration.h"
#include "Material.h"

template<>
InputParameters validParams<ConcreteThermalTimeIntegration>()
{
  InputParameters params = validParams<TimeDerivative>();
  params.addParam<std::string>("property_name", "thermal_capacity", "Heat capacity material property");
  return params;
}

ConcreteThermalTimeIntegration::ConcreteThermalTimeIntegration(const InputParameters & parameters) :
    TimeDerivative(parameters),
    _thermal_capacity(getMaterialProperty<Real>("thermal_capacity"))
{
}

Real
ConcreteThermalTimeIntegration::computeQpResidual()
{
  return _thermal_capacity[_qp] * TimeDerivative::computeQpResidual(); //self accumulation term
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
