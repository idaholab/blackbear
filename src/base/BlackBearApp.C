#include "BlackBearApp.h"
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
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  BlackBearApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  BlackBearApp::associateSyntax(_syntax, _action_factory);
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

void
BlackBearApp::registerObjectDepends(Factory & /*factory*/)
{
}

// External entry point for dynamic object registration
extern "C" void
BlackBearApp__registerObjects(Factory & factory)
{
  BlackBearApp::registerObjects(factory);
}
void
BlackBearApp::registerObjects(Factory & /*factory*/)
{
}

void
BlackBearApp::associateSyntaxDepends(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}

// External entry point for dynamic syntax association
extern "C" void
BlackBearApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  BlackBearApp::associateSyntax(syntax, action_factory);
}
void
BlackBearApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
