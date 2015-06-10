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

#ifndef CONCRETELATENTHEAT_H
#define CONCRETELATENTHEAT_H

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
  const MaterialProperty<Real> & _ca;
  const MaterialProperty<Real> & _moisture_capacity;

  // std::vector<Real> _sto_v;

  /// Coupled time derivatives of moisture.
  std::vector<VariableValue *> _dvals_dt;
};

#endif //CONCRETELATENTHEAT_H
