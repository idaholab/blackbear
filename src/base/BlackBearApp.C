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
  BlackBearApp::registerObjectDepends(_factory);
  BlackBearApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  BlackBearApp::associateSyntaxDepends(_syntax, _action_factory);
  BlackBearApp::associateSyntax(_syntax, _action_factory);

  Moose::registerExecFlags(_factory);
  ModulesApp::registerExecFlags(_factory);
  BlackBearApp::registerExecFlags(_factory);
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
BlackBearApp::registerObjects(Factory & factory)
{
  Registry::registerObjectsTo(factory, {"BlackBearApp"});
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
BlackBearApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  Registry::registerActionsTo(action_factory, {"BlackBearApp"});

  registerTask("add_primary_aqueous_species", false);
  addTaskDependency("add_primary_aqueous_species", "init_displaced_problem");
  addTaskDependency("setup_variable_complete", "add_primary_aqueous_species");

  registerTask("add_initial_primary_species_conc", false);
  addTaskDependency("add_initial_primary_species_conc", "set_adaptivity_options");
  addTaskDependency("add_preconditioning", "add_initial_primary_species_conc");

  registerTask("add_primary_species_kernels", false);
  addTaskDependency("add_primary_species_kernels", "add_vector_postprocessor");
  addTaskDependency("check_output", "add_primary_species_kernels");

  registerTask("add_secondary_species_kernels", false);
  addTaskDependency("add_secondary_species_kernels", "add_vector_postprocessor");
  addTaskDependency("check_output", "add_secondary_species_kernels");

  registerTask("add_minerals_aux_vars", false);
  addTaskDependency("add_minerals_aux_vars", "init_displaced_problem");
  addTaskDependency("setup_variable_complete", "add_minerals_aux_vars");

  registerTask("add_minerals_initial_conc", false);
  addTaskDependency("add_minerals_initial_conc", "set_adaptivity_options");
  addTaskDependency("add_preconditioning", "add_minerals_initial_conc");

  registerTask("add_minerals_kernels", false);
  addTaskDependency("add_minerals_kernels", "add_vector_postprocessor");
  addTaskDependency("check_output", "add_minerals_kernels");

  registerTask("add_minerals_auxkernels", false);
  addTaskDependency("add_minerals_auxkernels", "add_vector_postprocessor");
  addTaskDependency("check_output", "add_minerals_auxkernels");

  syntax.registerActionSyntax(
      "SetReactionNetworkAction", "ReactionNetwork", "add_primary_aqueous_species");
  syntax.registerActionSyntax(
      "SetReactionNetworkAction", "ReactionNetwork", "add_initial_primary_species_conc");
  syntax.registerActionSyntax(
      "SetReactionNetworkAction", "ReactionNetwork", "add_primary_species_kernels");

  syntax.registerActionSyntax(
      "SetReactionNetworkAction", "ReactionNetwork", "add_secondary_species_kernels");

  syntax.registerActionSyntax(
      "SetReactionNetworkAction", "ReactionNetwork", "add_minerals_aux_vars");
  syntax.registerActionSyntax(
      "SetReactionNetworkAction", "ReactionNetwork", "add_minerals_initial_conc");

  syntax.registerActionSyntax(
      "SetReactionNetworkAction", "ReactionNetwork", "add_minerals_kernels");
  syntax.registerActionSyntax(
      "SetReactionNetworkAction", "ReactionNetwork", "add_minerals_auxkernels");
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
}
