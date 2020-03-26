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

class BlackBearApp;

template <>
InputParameters validParams<BlackBearApp>();

class BlackBearApp : public MooseApp
{
public:
  BlackBearApp(InputParameters parameters);
  virtual ~BlackBearApp();

  static void registerApps();
  static void registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax);
};
