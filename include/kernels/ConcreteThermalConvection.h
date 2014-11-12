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
