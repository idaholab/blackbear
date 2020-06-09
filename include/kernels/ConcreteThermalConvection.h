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

class ConcreteThermalConvection : public Kernel
{
public:
  static InputParameters validParams();
  ConcreteThermalConvection(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  const MaterialProperty<Real> & _cw;
  const MaterialProperty<Real> & _Dh;
  const VariableGradient & _grad_rh;
  unsigned int _rh_var;
  // const MaterialProperty<RealGradient> & _darcy_moisture_flux;
  // MooseArray<RealGradient> &_pore_velocity_water;
};
