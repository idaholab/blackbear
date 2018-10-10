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

#ifndef BLACK_BEARAPP_H
#define BLACK_BEARAPP_H

#include "MooseApp.h"

class BlackBearApp;

template <>
InputParameters validParams<BlackBearApp>();

class BlackBearApp : public MooseApp
{
public:
  BlackBearApp(InputParameters parameters);
  virtual ~BlackBearApp();

  static void registerApps();
  ///@{ Deprecated registration functions
  static void registerObjects(Factory & factory);
  static void registerObjectDepends(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
  static void associateSyntaxDepends(Syntax & syntax, ActionFactory & action_factory);
  static void registerExecFlags(Factory & factory);
  ///@}
  //
  static void registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax);
};

#endif /* BLACK_BEARAPP_H */
