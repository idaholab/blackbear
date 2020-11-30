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

#include "MooseApp.h"



class BlackBearApp : public MooseApp
{
public:
  static InputParameters validParams();
  BlackBearApp(InputParameters parameters);
  virtual ~BlackBearApp();

  static void registerApps();
  static void registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax);
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);

  static void registerExecFlags(Factory & factory);
};
