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

#include "ConcreteMoistureDehydration.h"
#include "Material.h"

template<>
InputParameters validParams<ConcreteMoistureDehydration>()
{
  InputParameters params = validParams<TimeDerivative>();
  params.addParam<std::string>("property_name","hydrated_water","The water chemically bonded to hydrates");
  params.addParam<Real>("dehydration_start_temperature", 120.0, "themprature threshold for dehydartion of cement");
  params.addCoupledVar("temperature", "nonlinear variable name for temperature in unit of Celscius");
  return params;
}

ConcreteMoistureDehydration::ConcreteMoistureDehydration(const std::string & name, InputParameters parameters) :
    TimeDerivative(name, parameters),
    _T_dehydration(getParam<Real>("dehydration_start_temperature")),
    _WH(getMaterialProperty<Real>(getParam<std::string>("property_name"))),
    _WH_old(getMaterialPropertyOld<Real>(getParam<std::string>("property_name"))),

    _has_temperature(isCoupled("temperature")),
    _T(_has_temperature ? coupledValue("temperature") : _zero)

{
}

Real
ConcreteMoistureDehydration::computeQpResidual()
{
  Real _re = 0.0;

//   std:: cout <<_dt<<std::endl;

  if (_T[_qp] >= 120.0)
  {
    _re = (_WH[_qp] - _WH_old[_qp])/_dt * 0.05/600.0 * _T[_qp] * _test[_i][_qp];
  }



  return _re;
}

Real
ConcreteMoistureDehydration::computeQpJacobian()
{
  return 0.0;
}

Real ConcreteMoistureDehydration::computeQpOffDiagJacobian(unsigned int /*jvar*/)
{
  return 0.0;
}
