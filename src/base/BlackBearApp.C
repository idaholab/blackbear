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

InputParameters
BlackBearApp::validParams()
{
  InputParameters params = MooseApp::validParams();
  return params;
}

BlackBearApp::BlackBearApp(InputParameters parameters) : MooseApp(parameters)
{
  BlackBearApp::registerAll(_factory, _action_factory, _syntax);
}

BlackBearApp::~BlackBearApp() {}

static void
associateSyntaxInner(Syntax & syntax, ActionFactory & /*action_factory*/)
{
  registerSyntax("EmptyAction", "Constraints/EqualValueEmbeddedConstraint");
  registerSyntax("EqualValueEmbeddedConstraintAction", "Constraints/EqualValueEmbeddedConstraint/*");
}

void
BlackBearApp::registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax)
{
  Registry::registerObjectsTo(factory, {"BlackBearApp"});
  Registry::registerActionsTo(action_factory, {"BlackBearApp"});
  associateSyntaxInner(syntax, action_factory);

  ModulesApp::registerAll(factory, action_factory, syntax);
}

void
BlackBearApp::registerApps()
{
  registerApp(BlackBearApp);
}

// External entry point for object registration
extern "C" void
BlackBearApp__registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax)
{
  BlackBearApp::registerAll(factory, action_factory, syntax);
}

// External entry point for dynamic application loading
extern "C" void
BlackBearApp__registerApps()
{
  BlackBearApp::registerApps();
}
