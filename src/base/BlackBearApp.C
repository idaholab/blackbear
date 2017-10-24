#include "BlackBearApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

#include "ContactApp.h"
#include "HeatConductionApp.h"
#include "MiscApp.h"
#include "SolidMechanicsApp.h"
#include "TensorMechanicsApp.h"
#include "XFEMApp.h"

// concrete aqueous diffusion and reaction
#include "PorousMediaBase.h"
#include "MineralDissolutionPrecipAux.h"
#include "PrimaryAqueousSpeciesTimeIntegration.h"
#include "MineralSolutionTimeIntegration.h"
#include "SecondaryAqueousSpeciesTimeIntegration.h"
#include "PrimaryAqueousSpeciesDiffusion.h"
#include "SecondaryAqueousSpeciesDiffusion.h"
#include "SetReactionNetworkAction.h"

// concrete coupled moisture and heat diffusion
#include "ConcreteThermalTimeIntegration.h"
#include "ConcreteThermalConduction.h"
#include "ConcreteThermalConvection.h"
#include "ConcreteLatentHeat.h"
#include "ConcreteMoistureTimeIntegration.h"
#include "ConcreteMoistureDiffusion.h"
#include "ConcreteMoistureDehydration.h"
#include "SpecifiedVaporPressureBC.h"

// concrete ASR material model
#include "ConcreteElasticASR.h"
#include "ConcreteElasticASRModel.h"
#include "VSwellingASR.h"

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
BlackBearApp::registerObjectDepends(Factory & factory)
{
  SolidMechanicsApp::registerObjects(factory);
  TensorMechanicsApp::registerObjects(factory);
  ContactApp::registerObjects(factory);
  HeatConductionApp::registerObjects(factory);
  MiscApp::registerObjects(factory);
  XFEMApp::registerObjects(factory);
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
  registerAux(MineralDissolutionPrecipAux);

  registerKernel(PrimaryAqueousSpeciesTimeIntegration);
  registerKernel(MineralSolutionTimeIntegration);
  registerKernel(SecondaryAqueousSpeciesTimeIntegration);

  registerKernel(PrimaryAqueousSpeciesDiffusion);
  registerKernel(SecondaryAqueousSpeciesDiffusion);

  registerKernel(ConcreteThermalTimeIntegration);
  registerKernel(ConcreteThermalConduction);
  registerKernel(ConcreteThermalConvection);
  registerKernel(ConcreteLatentHeat);

  registerKernel(ConcreteMoistureTimeIntegration);
  registerKernel(ConcreteMoistureDiffusion);
  registerKernel(ConcreteMoistureDehydration);
  registerBoundaryCondition(SpecifiedVaporPressureBC);

  registerMaterial(PorousMediaBase);

  registerMaterial(ConcreteElasticASR);
  registerMaterial(ConcreteElasticASRModel);
  registerMaterial(VSwellingASR);
}

void
BlackBearApp::associateSyntaxDepends(Syntax & syntax, ActionFactory & action_factory)
{
  SolidMechanicsApp::associateSyntax(syntax, action_factory);
  TensorMechanicsApp::associateSyntax(syntax, action_factory);
  ContactApp::associateSyntax(syntax, action_factory);
  HeatConductionApp::associateSyntax(syntax, action_factory);
  MiscApp::associateSyntax(syntax, action_factory);
  XFEMApp::associateSyntax(syntax, action_factory);
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

  registerAction(SetReactionNetworkAction, "add_primary_aqueous_species");
  registerAction(SetReactionNetworkAction, "add_initial_primary_species_conc");
  registerAction(SetReactionNetworkAction, "add_primary_species_kernels");

  registerAction(SetReactionNetworkAction, "add_secondary_species_kernels");

  registerAction(SetReactionNetworkAction, "add_minerals_aux_vars");
  registerAction(SetReactionNetworkAction, "add_minerals_initial_conc");

  registerAction(SetReactionNetworkAction, "add_minerals_kernels");
  registerAction(SetReactionNetworkAction, "add_minerals_auxkernels");
}
