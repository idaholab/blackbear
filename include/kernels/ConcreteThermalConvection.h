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

#include "Kernel.h"
#include "Material.h"

// Forward Declarations
class ConcreteThermalConvection;

template <>
InputParameters validParams<ConcreteThermalConvection>();

class ConcreteThermalConvection : public Kernel
{
public:
  ConcreteThermalConvection(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _cw;
  const MaterialProperty<RealGradient> & _darcy_moisture_flux;
  // MooseArray<RealGradient> &_pore_velocity_water;
};
