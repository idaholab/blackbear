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
  params.set<bool>("use_legacy_material_output") = false;
  params.addCommandLineParam<bool>("neml2_only",
                                   "--neml2-only",
                                   false,
                                   "Executes the [NEML2] block in the input file and terminate.");
  return params;
}

BlackBearApp::BlackBearApp(InputParameters parameters) : MooseApp(parameters)
{
  BlackBearApp::registerAll(_factory, _action_factory, _syntax);
}

BlackBearApp::~BlackBearApp() {}

void
BlackBearApp::setupOptions()
{
  MooseApp::setupOptions();

#ifdef NEML2_ENABLED
  if (getParam<bool>("neml2_only"))
  {
    // Let parse_neml2 run before anything else, and stop after that.
    syntax().addDependency("determine_system_type", "parse_neml2");
    actionWarehouse().setFinalTask("parse_neml2");
  }
#endif
}

void
BlackBearApp::runInputFile()
{
  MooseApp::runInputFile();

#ifdef NEML2_ENABLED
  if (getParam<bool>("neml2_only"))
    _ready_to_exit = true;
#endif
}

static void
associateSyntaxInner(Syntax & syntax, ActionFactory & /*action_factory*/)
{
#ifdef NEML2_ENABLED
  registerTask("parse_neml2", /*required=*/true);
  syntax.addDependency("add_material", "parse_neml2");
  syntax.addDependency("add_user_object", "parse_neml2");
  registerSyntax("NEML2Action", "NEML2");
#endif
}

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

// External entry point for object registration
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
  associateSyntaxInner(syntax, action_factory);

  ModulesApp::registerAllObjects<BlackBearApp>(factory, action_factory, syntax);
}
