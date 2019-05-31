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

#ifndef TESTCONCRETEEXPANSIONEIGENSTRAIN_H
#define TESTCONCRETEEXPANSIONEIGENSTRAIN_H

#include "ConcreteExpansionEigenstrainBase.h"

// forward declarations
class TestConcreteExpansionEigenstrain;
class Function;

template <>
InputParameters validParams<TestConcreteExpansionEigenstrain>();

/**
 * Imposes a ConcreteExpansionEigenstrain in which the volumetric strain is
 * provided by a function of time (and possibly space) defined by the user.
 */
class TestConcreteExpansionEigenstrain : public ConcreteExpansionEigenstrainBase
{
public:
  TestConcreteExpansionEigenstrain(const InputParameters & parameters);

protected:
  virtual Real computeQpVolumetricStrain();
  virtual bool needStressEigenvalues() { return false; }

  /// The function that defines the volumetric strain
  const Function & _function;
};

#endif // TESTCONCRETEEXPANSIONEIGENSTRAIN_H
