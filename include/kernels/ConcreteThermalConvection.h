/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                        Grizzly                               */
/*                                                              */
/*           (c) 2015 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef CONCRETETHERMALCONVECTION
#define CONCRETETHERMALCONVECTION

#include "Kernel.h"
#include "Material.h"

//Forward Declarations
class ConcreteThermalConvection;

template<>
InputParameters validParams<ConcreteThermalConvection>();

class ConcreteThermalConvection : public Kernel
{
public:

  ConcreteThermalConvection(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  MaterialProperty<Real> & _cw;
  MaterialProperty<RealGradient> & _darcy_moisture_flux;
//  MooseArray<RealGradient> &_pore_velocity_water;

};
#endif //CONCRETETHERMALCONVECTION
