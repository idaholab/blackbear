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

#include "ConcreteASRMicrocrackingDamage.h"

registerMooseObject("BlackBearApp", ConcreteASRMicrocrackingDamage);

template <>
InputParameters
validParams<ConcreteASRMicrocrackingDamage>()
{
  InputParameters params = validParams<ScalarDamageBase>();
  params.addClassDescription("Scalar damage model based on ASR extent");
  params.addRequiredRangeCheckedParam<Real>(
      "residual_youngs_modulus_fraction",
      "residual_youngs_modulus_fraction >= 0.0 & residual_youngs_modulus_fraction <= 1.0",
      "Residual fraction of youngs_modulus at full ASR reaction");
  return params;
}

ConcreteASRMicrocrackingDamage::ConcreteASRMicrocrackingDamage(const InputParameters & parameters)
  : ScalarDamageBase(parameters),
    _beta_e(getParam<Real>("residual_youngs_modulus_fraction")),
    _ASR_extent(getMaterialProperty<Real>("ASR_extent"))
{
}

void
ConcreteASRMicrocrackingDamage::updateQpDamageIndex()
{
  _damage_index[_qp] = std::max(_damage_index_old[_qp], (1.0 - _beta_e) * _ASR_extent[_qp]);
}
