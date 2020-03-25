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

#include "BlackBearApp.h"

class BlackBearTestApp;

template <>
InputParameters validParams<BlackBearTestApp>();

class BlackBearTestApp : public BlackBearApp
{
public:
  BlackBearTestApp(InputParameters parameters);
  virtual ~BlackBearTestApp();

  static void registerApps();
  static void registerAll(Factory & factory,
                          ActionFactory & action_factory,
                          Syntax & syntax,
                          bool use_test_objs = false);
};
