//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "NEMLMaterialPropertyReset.h"

registerMooseObject("BlackBearApp", NEMLMaterialPropertyReset);

InputParameters
NEMLMaterialPropertyReset::validParams()
{
  InputParameters params = ElementUserObject::validParams();

  params.addCoupledVar("temperature", 0.0, "Temperature field");
  params.addParam<Real>("critical_temperature", 1000.0, "Annealing temperature");

  params.addRequiredParam<std::vector<std::string>>("properties", "Properties to reset");
  params.addRequiredParam<MaterialName>("material", "The NEML material object to reset");

  return params;
}

NEMLMaterialPropertyReset::NEMLMaterialPropertyReset(const InputParameters & parameters)
  : ElementUserObject(parameters),
    _temperature(coupledValue("temperature")),
    _critical_temperature(getParam<Real>("critical_temperature")),
    _props(getParam<std::vector<std::string>>("properties"))
{
}

void
NEMLMaterialPropertyReset::initialSetup()
{
  _neml_material = dynamic_cast<CauchyStressFromNEML *>(&getMaterial("material"));
  if (_neml_material == nullptr)
    mooseError("Unable to link NEMLMaterialPropertyReset object to the "
               "stress calculator");
}

void
NEMLMaterialPropertyReset::initialize()
{
}

void
NEMLMaterialPropertyReset::execute()
{
  for (_qp = 0; _qp < _qrule->n_points(); _qp++)
  {
    resetQp();
  }
}

void
NEMLMaterialPropertyReset::resetQp()
{
  if (_temperature[_qp] >= _critical_temperature)
  {
    _neml_material->reset_state(_props, _qp);
  }
}

void
NEMLMaterialPropertyReset::finalize()
{
}

void
NEMLMaterialPropertyReset::threadJoin(const UserObject & y)
{
}
