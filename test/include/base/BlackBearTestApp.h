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

#ifndef BLACK_BEARTESTAPP_H
#define BLACK_BEARTESTAPP_H

#include "MooseApp.h"

class BlackBearTestApp;

template <>
InputParameters validParams<BlackBearTestApp>();

class BlackBearTestApp : public MooseApp
{
public:
  BlackBearTestApp(InputParameters parameters);
  virtual ~BlackBearTestApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
  static void registerExecFlags(Factory & factory);
};

#endif /* BLACK_BEARTESTAPP_H */
