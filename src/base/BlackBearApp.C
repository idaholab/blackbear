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
  return params;
}

BlackBearApp::BlackBearApp(const InputParameters & parameters) : MooseApp(parameters)
{
  BlackBearApp::registerAll(_factory, _action_factory, _syntax);
}

BlackBearApp::~BlackBearApp() {}

void
BlackBearApp::setupOptions()
{
  MooseApp::setupOptions();
}

void
BlackBearApp::runInputFile()
{
  MooseApp::runInputFile();
}

static void
associateSyntaxInner(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
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
  {
    const std::string doc = "New Engineering Material model Library, version 1 ";
#ifdef NEML_ENABLED
    addCapability("neml", true, doc + "is available.");
#else
    addCapability("neml", false, doc + "is not available.");
#endif
  }

  registerApp(BlackBearApp);
  ModulesApp::registerApps();
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

  // Spatio-temporal paths have their own task, which only accepts SpatioTemporalPath objects.
  // Paths are created before the user objects, mesh modifiers, and materials that use them.
  registerMooseObjectTask("add_spatiotemporal_path", SpatioTemporalPath, false);
  syntax.addDependency("add_spatiotemporal_path", "create_problem_complete");
  syntax.addDependency("add_user_object", "add_spatiotemporal_path");
  syntax.addDependency("add_mesh_modifier", "add_spatiotemporal_path");
  syntax.addDependency("add_material", "add_spatiotemporal_path");

  // Adds [SpatioTemporalPaths] block
  registerSyntax("EmptyAction", "SpatioTemporalPaths");
  registerSyntaxTask(
      "AddSpatioTemporalPathAction", "SpatioTemporalPaths/*", "add_spatiotemporal_path");

  // Adds [SpatioTemporalHeat] block, with the path defined in its [path] sub-block
  registerSyntax("SpatioTemporalHeatAction", "SpatioTemporalHeat");
  registerSyntaxTask(
      "AddSpatioTemporalPathAction", "SpatioTemporalHeat/path", "add_spatiotemporal_path");

  ModulesApp::registerAllObjects<BlackBearApp>(factory, action_factory, syntax);
}
