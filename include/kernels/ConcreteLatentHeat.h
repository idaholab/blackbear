#ifndef CONCRETELATENTHEAT
#define CONCRETELATENTHEAT

#include "TimeDerivative.h"

// Forward Declaration
class ConcreteLatentHeat;

/**
 * validParams returns the parameters that this Kernel accepts / needs
 * The actual body of the function MUST be in the .C file.
 */
template<>
InputParameters validParams<ConcreteLatentHeat>();

/**
 * Define the Kernel for a CoupledConvectionReactionSub operator that looks like:
 *
 * storage * delta pressure / delta t
 *
 * This first line is defining the name and inheriting from Kernel.
 */
class ConcreteLatentHeat : public TimeDerivative
{
public:

  ConcreteLatentHeat(const std::string & name, InputParameters parameters);

protected:
  /**
   * Responsible for computing the residual at one quadrature point
   *
   * This should always be defined in the .C
   * @return The residual of mass accumulation of primary species concentration.
   */
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  /// Material property of porosity
  MaterialProperty<Real> & _ca;
  MaterialProperty<Real> & _moisture_capacity;

//  std::vector<Real> _sto_v;
  /// Coupled time derivatives of moisture.
  std::vector<VariableValue *> _dvals_dt;

};

#endif // CONCRETELATENTHEAT
