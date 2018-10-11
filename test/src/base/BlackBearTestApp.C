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
#include "AppFactory.h"

template <>
InputParameters
validParams<BlackBearTestApp>()
{
  InputParameters params = validParams<BlackBearApp>();
  return params;
}

BlackBearTestApp::BlackBearTestApp(InputParameters parameters) : BlackBearApp(parameters)
{
  BlackBearTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

BlackBearTestApp::~BlackBearTestApp() {}

// External entry point for dynamic application loading
extern "C" void
BlackBearTestApp__registerApps()
{
  BlackBearTestApp::registerApps();
}
void
BlackBearTestApp::registerApps()
{
  registerApp(BlackBearApp);
  registerApp(BlackBearTestApp);
}

// External entry point for object registration
extern "C" void
BlackBearTestApp__registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax)
{
  BlackBearTestApp::registerAll(factory, action_factory, syntax);
}
void
BlackBearTestApp::registerAll(Factory & factory,
                              ActionFactory & action_factory,
                              Syntax & syntax,
                              bool use_test_objs)
{
  BlackBearApp::registerAll(factory, action_factory, syntax);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(factory, {"BlackBearTestApp"});
    Registry::registerActionsTo(action_factory, {"BlackBearTestApp"});
  }
}
