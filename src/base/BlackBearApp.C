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

#include "BlackBearApp.h"
#include "BlackBearSyntax.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<BlackBearApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

BlackBearApp::BlackBearApp(InputParameters parameters) : MooseApp(parameters)
{
  BlackBearApp::registerAll(_factory, _action_factory, _syntax);
}

BlackBearApp::~BlackBearApp() {}

// External entry point for dynamic application loading
extern "C" void
BlackBearApp__registerApps()
{
  BlackBearApp::registerApps();
}
void
BlackBearApp::registerApps()
{
  registerApp(BlackBearApp);
}

extern "C" void
BlackBearApp__registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax)
{
  BlackBearApp::registerAll(factory, action_factory, syntax);
}
void
BlackBearApp::registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax)
{
  Registry::registerObjectsTo(factory, {"BlackBearApp"});
  Registry::registerActionsTo(action_factory, {"BlackBearApp"});
  BlackBear::associateSyntax(syntax, action_factory);

  ModulesApp::registerAll(factory, action_factory, syntax);
}

void
BlackBearApp::registerObjectDepends(Factory & /*factory*/)
{
  mooseDeprecated("use registerAll instead of registerObjectDepends");
}

// External entry point for dynamic object registration
extern "C" void
BlackBearApp__registerObjects(Factory & factory)
{
  BlackBearApp::registerObjects(factory);
}
void
BlackBearApp::registerObjects(Factory & factory)
{
  mooseDeprecated("use registerAll instead of registerObjects");
  Registry::registerObjectsTo(factory, {"BlackBearApp"});
}

void
BlackBearApp::associateSyntaxDepends(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
  mooseDeprecated("use registerAll instead of associateSyntaxDepends");
}

// External entry point for dynamic syntax association
extern "C" void
BlackBearApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  BlackBearApp::associateSyntax(syntax, action_factory);
}
void
BlackBearApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  mooseDeprecated("use registerAll instead of associateSyntax");
  Registry::registerActionsTo(action_factory, {"BlackBearApp"});
  BlackBear::associateSyntax(syntax, action_factory);
}

// External entry point for dynamic execute flag registration
extern "C" void
BlackBearApp__registerExecFlags(Factory & factory)
{
  BlackBearApp::registerExecFlags(factory);
}
void
BlackBearApp::registerExecFlags(Factory & /*factory*/)
{
  mooseDeprecated("use registerAll instead of registerExecFlags");
}
