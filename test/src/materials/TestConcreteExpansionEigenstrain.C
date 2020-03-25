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

#include "TestConcreteExpansionEigenstrain.h"
#include "Function.h"

registerMooseObject("BlackBearTestApp", TestConcreteExpansionEigenstrain);

template <>
InputParameters
validParams<TestConcreteExpansionEigenstrain>()
{
  InputParameters params = validParams<ConcreteExpansionEigenstrainBase>();
  params.addRequiredParam<FunctionName>("function",
                                        "name of the function used for the volumetric strain");
  return params;
}

TestConcreteExpansionEigenstrain::TestConcreteExpansionEigenstrain(
    const InputParameters & parameters)
  : ConcreteExpansionEigenstrainBase(parameters, std::string("test")),
    _function(getFunction("function"))
{
}

Real
TestConcreteExpansionEigenstrain::computeQpVolumetricStrain()
{
  return _function.value(_t, _q_point[_qp]);
}
