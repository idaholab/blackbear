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

#include "FunctionOffsetDirichletBC.h"
#include "Function.h"

registerMooseObject("BlackBearApp", FunctionOffsetDirichletBC);

InputParameters
FunctionOffsetDirichletBC::validParams()
{
  InputParameters params = DirichletBCBase::validParams();
  params.addRequiredParam<FunctionName>("function", "The forcing function.");
  params.addClassDescription(
      "Imposes the essential boundary condition $u=g(t,\\vec{x})$, where $g$ "
      "is a (possibly) time and space-dependent MOOSE Function, but ofsetting the location where "
      "the function is evaluated.");
  params.addCoupledVar("nx", "x-component of the normal");
  params.setDocUnit("nx", "unitless");
  params.addCoupledVar("ny", "y-component of the normal");
  params.setDocUnit("ny", "unitless");
  params.addCoupledVar("nz", "z-component of the normal");
  params.setDocUnit("nz", "unitless");

  params.set<std::vector<VariableName>>("nx") = {"nodal_normal_x"};
  params.set<std::vector<VariableName>>("ny") = {"nodal_normal_y"};
  params.set<std::vector<VariableName>>("nz") = {"nodal_normal_z"};

  params.addParam<Real>(
      "offset", 0.0, "Offset of the function evaluation location in the direction of nodal normal");
  params.setDocUnit("offset", "length");
  return params;
}

FunctionOffsetDirichletBC::FunctionOffsetDirichletBC(const InputParameters & parameters)
  : DirichletBCBase(parameters),
    _func(getFunction("function")),
    _nx(coupledValue("nx")),
    _ny(coupledValue("ny")),
    _nz(coupledValue("nz")),
    _offset(getParam<Real>("offset"))
{
}

Real
FunctionOffsetDirichletBC::computeQpValue()
{
  _nor = Point(_nx[_qp], _ny[_qp], _nz[_qp]);
  return _func.value(_t, (*_current_node + _offset * _nor));
}
