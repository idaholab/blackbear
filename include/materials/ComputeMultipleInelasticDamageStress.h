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

#pragma once

#include "ComputeMultipleInelasticStress.h"

/**
 * This ComputeMultipleInelasticStress is to be used with DamagePlasticityStressUpdate
 */
class ComputeMultipleInelasticDamageStress : public ComputeMultipleInelasticStress
{
public:
  static InputParameters validParams();
  ComputeMultipleInelasticDamageStress(const InputParameters & parameters);
  virtual void initialSetup() override;
protected:
  /// damage parameter for DamagePlasticityStressUpdate model
  const MaterialProperty<Real> & _D;
  const MaterialProperty<Real> & _D_old;

  virtual void computeQpJacobianMult() override;

  virtual void computeAdmissibleState(unsigned model_number,
                                      RankTwoTensor & elastic_strain_increment,
                                      RankTwoTensor & inelastic_strain_increment,
                                      RankFourTensor & consistent_tangent_operator) override;

  virtual void
  updateQpStateSingleModel(unsigned model_number,
                           RankTwoTensor & elastic_strain_increment,
                           RankTwoTensor & combined_inelastic_strain_increment) override;
};
