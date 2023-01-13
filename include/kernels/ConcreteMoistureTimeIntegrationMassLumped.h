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

// MOOSE includes
#include "MassLumpedTimeDerivative.h"

// Forward Declarations
class ConcreteMoistureTimeIntegrationMassLumped;

/**
 * Time derivative term in the governing equation for moisture transport
 * in concrete, with mass lumping.
 */
class ConcreteMoistureTimeIntegrationMassLumped : public MassLumpedTimeDerivative
{
public:
  static InputParameters validParams();
  ConcreteMoistureTimeIntegrationMassLumped(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  /// Material property for moisture capacity (dW/dH). Note that only
  /// a subset of the moisture diffusivity models provide this.
  /// The other models assume that this is 1.
  const MaterialProperty<Real> * _moisture_capacity; //  dW/dH
};
