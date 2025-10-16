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

#include "ConcreteDryingShrinkage.h"
#include "RankTwoTensor.h"

registerMooseObject("BlackBearApp", ConcreteDryingShrinkage);

InputParameters
ConcreteDryingShrinkage::validParams()
{
  InputParameters params = ComputeEigenstrainBase::validParams();

  params.addClassDescription("Computes concrete drying shrinkage");
  params.addRequiredCoupledVar("humidity", "relative humidity");
  params.addRangeCheckedParam<Real>("drying_shrinkage_coefficient",
                                    "drying_shrinkage_coefficient<0",
                                    "total shrinkage at 0 relative humidity");
  params.setDocUnit("drying_shrinkage_coefficient", "unitless");
  params.addRangeCheckedParam<Real>(
      "exponent", 1, "exponent>0", "exponent of the shrinkage law, default: 1");
  params.setDocUnit("exponent", "unitless");
  params.addRangeCheckedParam<Real>("irreversibility_threshold",
                                    "0<=irreversibility_threshold<=1",
                                    "humidity below which shrinkage becomes irreversible");
  params.setDocUnit("irreversibility_threshold", "unitless");
  return params;
}

ConcreteDryingShrinkage::ConcreteDryingShrinkage(const InputParameters & parameters)
  : ComputeEigenstrainBase(parameters),
    _humidity(coupledValue("humidity")),
    _shrinkage_coefficient(getParam<Real>("drying_shrinkage_coefficient")),
    _exponent(getParam<Real>("exponent")),
    _is_irreversible(isParamValid("irreversibility_threshold")),
    _irreversibility_threshold(isParamValid("irreversibility_threshold")
                                   ? getParam<Real>("irreversibility_threshold")
                                   : 0),
    _irreversible_shrinkage(_is_irreversible ? &declareProperty<Real>("irreversible_shrinkage")
                                             : NULL),
    _irreversible_shrinkage_old(
        _is_irreversible ? &getMaterialPropertyOld<Real>("irreversible_shrinkage") : NULL)
{
}

void
ConcreteDryingShrinkage::initQpStatefulProperties()
{
  ComputeEigenstrainBase::initQpStatefulProperties();

  if (_is_irreversible)
    (*_irreversible_shrinkage)[_qp] = 0;
}

void
ConcreteDryingShrinkage::computeQpEigenstrain()
{
  _eigenstrain[_qp].zero();

  Real shrinkage = 0;
  if (_is_irreversible)
  {
    (*_irreversible_shrinkage)[_qp] = (*_irreversible_shrinkage_old)[_qp];
    if (_humidity[_qp] > _irreversibility_threshold)
    {
      // we are still in the reversible regime
      shrinkage = std::min((*_irreversible_shrinkage)[_qp],
                           std::pow(std::max((1. - _humidity[_qp]), 0.), _exponent) *
                               _shrinkage_coefficient);
    }
    else
    {
      // we need to check if the new humidity is lower than the previous
      Real reversible_shrinkage =
          std::pow(std::max((1. - _irreversibility_threshold), 0.), _exponent) *
          _shrinkage_coefficient;
      shrinkage = std::pow(std::max((1. - _humidity[_qp]), 0.), _exponent) * _shrinkage_coefficient;
      Real irreversible_shrinkage = shrinkage - reversible_shrinkage;
      if (irreversible_shrinkage < (*_irreversible_shrinkage)[_qp])
        (*_irreversible_shrinkage)[_qp] = irreversible_shrinkage;
    }
  }
  else
    shrinkage = std::pow(std::max((1. - _humidity[_qp]), 0.), _exponent) * _shrinkage_coefficient;

  // shrinkage is assumed isotropic
  _eigenstrain[_qp].addIa(shrinkage);
}
