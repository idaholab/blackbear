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

#include "ComputeMultipleInelasticDamageStress.h"
#include "StressUpdateBase.h"

registerMooseObject("BlackBearApp", ComputeMultipleInelasticDamageStress);

InputParameters
ComputeMultipleInelasticDamageStress::validParams()
{
  InputParameters params = ComputeMultipleInelasticStress::validParams();
  params.addClassDescription("This ComputeMultipleInelasticStress is to be used with "
                             "DamagePlasticityStressUpdate"); 
  return params;
}

ComputeMultipleInelasticDamageStress::ComputeMultipleInelasticDamageStress(
    const InputParameters & parameters)
  : ComputeMultipleInelasticStress(parameters),
    _D(getMaterialProperty<Real>("damage_variable")),
    _D_old(getMaterialPropertyOld<Real>("damage_variable"))
{
}

void
ComputeMultipleInelasticDamageStress::computeQpJacobianMult()
{
  ComputeMultipleInelasticStress::computeQpJacobianMult();
  _Jacobian_mult[_qp] = (1.0 - _D_old[_qp]) * _Jacobian_mult[_qp];
  // _Jacobian_mult[_qp] = (1.0 - _D[_qp]) * _Jacobian_mult[_qp];
}

void
ComputeMultipleInelasticDamageStress::updateQpStateSingleModel(
    unsigned model_number,
    RankTwoTensor & elastic_strain_increment,
    RankTwoTensor & combined_inelastic_strain_increment)
{
  ComputeMultipleInelasticStress::updateQpStateSingleModel(
      model_number, elastic_strain_increment, combined_inelastic_strain_increment);
  _Jacobian_mult[_qp] = (1.0 - _D_old[_qp]) * _Jacobian_mult[_qp];
}

void
ComputeMultipleInelasticDamageStress::computeAdmissibleState(
    unsigned model_number,
    RankTwoTensor & elastic_strain_increment,
    RankTwoTensor & inelastic_strain_increment,
    RankFourTensor & consistent_tangent_operator)
{
  _models[model_number]->updateState(elastic_strain_increment,
                                     inelastic_strain_increment,
                                     _rotation_increment[_qp],
                                     _stress[_qp],
                                     _stress_old[_qp] / (1.0 - _D_old[_qp]),
                                     // _stress_old[_qp] / (1.0 - _D[_qp]),
                                     _elasticity_tensor[_qp],
                                     _elastic_strain_old[_qp],
                                     _tangent_operator_type == TangentOperatorEnum::nonlinear,
                                     consistent_tangent_operator);
  _stress[_qp] *= (1.0 - _D_old[_qp]);
}
