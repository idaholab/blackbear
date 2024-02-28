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

#include "BlackBearTestApp.h"
#include "MooseMain.h"
#include "QuasiStaticSolidMechanicsPhysicsBase.h"

// Begin the main program.
int
main(int argc, char * argv[])
{
  // register inelastic strain
  QuasiStaticSolidMechanicsPhysicsBase::addCartesianComponentOutput("inelastic_strain");

  Moose::main<BlackBearTestApp>(argc, argv);

  return 0;
}
