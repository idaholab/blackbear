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

#include "ConcreteExpansionEigenstrainBase.h"

// forward declarations
class Function;


/**
 * Imposes a ConcreteExpansionEigenstrain in which the volumetric strain is
 * provided by a function of time (and possibly space) defined by the user.
 */
class TestConcreteExpansionEigenstrain : public ConcreteExpansionEigenstrainBase
{
public:
  static InputParameters validParams();
  TestConcreteExpansionEigenstrain(const InputParameters & parameters);

protected:
  virtual Real computeQpVolumetricStrain();
  virtual bool needStressEigenvalues() { return false; }

  /// The function that defines the volumetric strain
  const Function & _function;
};
