/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                        Grizzly                               */
/*                                                              */
/*           (c) 2015 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "SetReactionNetworkAction.h"
#include "AddAuxVariableAction.h"
// #include "Parser.h"
#include "FEProblem.h"

#include <sstream>
#include <stdexcept>

// libMesh includes
#include "libmesh/equation_systems.h"
#include "libmesh/nonlinear_implicit_system.h"
#include "libmesh/explicit_system.h"
#include "libmesh/string_to_enum.h"
#include "libmesh/fe.h"

// Regular expression includes
#include "pcrecpp.h"

template<>
InputParameters validParams<SetReactionNetworkAction>()
{
  InputParameters params = validParams<Action>();
  params.addRequiredParam<std::vector<NonlinearVariableName> >("primary_aqueous_species", "The list of primary variables to add");
  params.addRequiredParam<std::vector<Real> >("initial_condition", "initial values for the primary aqueous sepcies");

  params.addParam<std::vector<AuxVariableName> >("secondary_aqueous_species", "The list of secondary aquesous species to add");
  params.addParam<std::string>("aqueous_speciations", "The list of aqueous equilibrium reactions");

  params.addParam<std::vector<AuxVariableName> >("minerals", "The list of minerals adding to the aux variables ");
  params.addParam<std::vector<Real> >("initial_mineral_conc", "initial values for mineralsin  mol/L of solution ");

  params.addParam<std::vector<std::string> >("mineral_solution_reactions", "The list of mineral-solution reactions");
  params.addParam<std::vector<Real> >("log_k", "The list of equilibrium constants for all reactions");
  params.addParam<std::vector<Real> >("specific_reactive_surface_area", "The list of specific reactive surface area for all minerals, (m^2/L)");
  params.addParam<std::vector<Real> >("kinetic_rate_constant", "The list of kinetic rate constant for all reactions, (mol/m^2/s)");
  params.addParam<std::vector<Real> >("activation_energy", "The list of activation energy values for all reactions, (J/mol)");
  params.addParam<Real>("gas_constant", "Gas constant, 8.314 (J/mol/K)");
  params.addParam<Real>("reference_temperature", "The list of reference temperatures for all reactions, (K)");
  params.addParam<Real>("system_temperature", "The list of system temperatures for all reactions, (K)");

  MooseEnum familyEnum = AddAuxVariableAction::getAuxVariableFamilies();
  params.addParam<MooseEnum>("family", familyEnum, "Specifies the family of FE shape functions to use for the q AuxVariables");
  MooseEnum orderEnum = AddAuxVariableAction::getAuxVariableOrders();
  params.addParam<MooseEnum>("order", orderEnum,  "Specifies the order of the FE shape function to use for the q AuxVariables");

  return params;
}

SetReactionNetworkAction::SetReactionNetworkAction(InputParameters params) :
    Action(params),
    _fe_type(Utility::string_to_enum<Order>(getParam<MooseEnum>("order")),
             Utility::string_to_enum<FEFamily>(getParam<MooseEnum>("family")))
{
}

void
SetReactionNetworkAction::act()
{
  //
  // Setup primary aqeous species
  //

  // add primary aqueous species
  std::vector<NonlinearVariableName> nl_vars = getParam<std::vector<NonlinearVariableName> >("primary_aqueous_species");
  std::vector<Real> initial_vals = getParam<std::vector<Real> >("initial_condition");
  std::vector<AuxVariableName> aux_vars = getParam<std::vector<AuxVariableName> >("secondary_aqueous_species");
  std::vector<AuxVariableName> mineral_vars = getParam<std::vector<AuxVariableName> >("minerals");

  if (_current_task == "add_primary_aqueous_species" )
  {
    _console << std::endl;
    _console << "*********************************************************************************"<<std::endl;
    _console << "primary aqueous species:      ";

    for (unsigned int i=0; i < nl_vars.size(); i++)
    {
      _console << nl_vars[i] << "\t";
      Real scale_factor = 1.0;
      _problem->addVariable(nl_vars[i], _fe_type, scale_factor);
    }
    _console<<std::endl;
    _console << "---------------------------------------------------------------------------------"<<std::endl;
  }
  // setup initial concentrations for the primary aqueous species:
  else if (_current_task == "add_initial_primary_species_conc")
  {
    _console << std::endl;
    _console << "*********************************************************************************"<<std::endl;
    _console <<"Initial primary species concs: ";

    for (unsigned int i=0; i < nl_vars.size(); i++)
    {
      _console << initial_vals[i] << "\t";
      InputParameters params = _factory.getValidParams("ConstantIC");
      params.set<VariableName>("variable") = nl_vars[i];
      params.set<Real>("value") = initial_vals[i];
      _problem->addInitialCondition("ConstantIC", "ic_for" + nl_vars[i], params);
    }
    _console<<std::endl;
    _console << "---------------------------------------------------------------------------------"<<std::endl;

  }
  // add time integration & diffusion kernels for primary aqueous species
  else if (_current_task == "add_primary_species_kernels")
  {
#ifdef DEBUG
   _console << std::endl;
   _console << "*********************************************************************************"<<std::endl;
   _console <<"Adding kernels to primary aqueous species: ";
#endif

    for (unsigned int i=0; i < nl_vars.size(); i++)
    {

      InputParameters params_euler = _factory.getValidParams("PrimaryAqueousSpeciesTimeIntegration");
      params_euler.set<NonlinearVariableName>("variable") = nl_vars[i];
      params_euler.set<MaterialPropertyName>("property_name") = "porosity";
      _problem->addKernel("PrimaryAqueousSpeciesTimeIntegration", nl_vars[i] + "_timeintegration", params_euler);

      InputParameters params_diff = _factory.getValidParams("PrimaryAqueousSpeciesDiffusion");
      params_diff.set<NonlinearVariableName>("variable") = nl_vars[i];
      params_diff.set<MaterialPropertyName>("property_name") = "diffusivity";
      _problem->addKernel("PrimaryAqueousSpeciesDiffusion", nl_vars[i] + "_diffusion", params_diff);

    }
//    _console<<std::endl;
//    _console << "---------------------------------------------------------------------------------"<<std::endl;

  }
/*
 Setup aqueous equilibrium  speciation reactions
*/
  else if (_current_task == "add_secondary_species_kernels")
  {
    if (_pars.isParamValid("secondary_aqueous_species"))
    {
      if (aux_vars.size() == 0)
        mooseWarning("No equilibrium aqueous species provided!");

      // for (unsigned int i=0; i < aux_vars.size(); i++)
      // {
      //   _console << "secondary aqueous species: " << aux_vars[i] << "\t";
      //   FEType fe_type(Utility::string_to_enum<Order>("first"),
      //                  Utility::string_to_enum<FEFamily>("lagrange"));
      //   _problem->addAuxVariable(aux_vars[i], fe_type);
      // }
      //
      //  _console << std::endl;

      // setup aqueous speciation reaction network
      std::string reactions = getParam<std::string>("aqueous_speciations");


      // Getting ready for the parsing system
      pcrecpp::RE re_reactions("(.*?)"                 // the reaction network (any character until the equalibrium coefficient appears)
                           "\\s"                       // word boundary
                             "("                       // start capture
                               "-?"                    // optional minus sign
                                 "\\d+(?:\\.\\d*)?"    // digits followed by optional decimal and more 0 or more digits
                             ")"
                           "\\b"                       // word boundary
                           "(?:\\s+|$)"                // eat the whitespace
                           , pcrecpp::RE_Options().set_extended(true));

      pcrecpp::RE re_terms("(\\S+)");
      pcrecpp::RE re_coeff_and_species("(?: \\(? (.*?) \\)? )"  // match the leading coefficent
                                   "([A-Za-z].*)"           // match the species
                                   , pcrecpp::RE_Options().set_extended(true));

      pcrecpp::StringPiece input(reactions);

      pcrecpp::StringPiece single_reaction, term;
      Real equal_coeff;

      std::vector<std::vector<bool> > primary_participation(nl_vars.size());
      std::vector<string> eq_species;

      std::vector<Real> eq_const; //log10K eq. constant for each aqueous reaction
      std::vector<std::vector<Real> > sto_u(nl_vars.size());
      std::vector<std::vector<Real> > weight(nl_vars.size());
      std::vector<std::vector<std::vector<Real> > > sto_v(nl_vars.size());
      std::vector<std::vector<std::vector<VariableName> > > coupled_v(nl_vars.size());

      std::vector<std::vector<Real> > stos;                             //per aqueous speciation reaction
      std::vector<std::vector<std::string> > primary_aqueous_species_involved;  //per aqueous speciation reaction
      unsigned int n_reactions = 0;

      // Start parsing
      // Going into every single reaction
      _console << std::endl;
      _console << "*********************************************************************************"<<std::endl;
      _console << "Aqueous sepciation reaction network:"<<std::endl;
      while (re_reactions.FindAndConsume(&input, &single_reaction, &equal_coeff))
      {
        n_reactions += 1;
        eq_const.push_back(equal_coeff);

        // capture all of the terms
        std::string species, coeff_str;
        Real coeff;
        int sign = 1;
        bool secondary = false;

        std::vector<Real> local_stos;
        std::vector<std::string> local_species_list;

        // Going to find every single term in this reaction, sto_species combos and operators
        while (re_terms.FindAndConsume(&single_reaction, &term))
        {
          // Separating the sto from species
          if (re_coeff_and_species.PartialMatch(term, &coeff_str, &species))
          {
            if (coeff_str.length())
            {
              std::istringstream iss(coeff_str);
              iss >> coeff;
            }
            else
              coeff = 1.0;

            coeff *= sign;

            if (secondary)
              eq_species.push_back(species);
            else
            {
              local_stos.push_back(coeff);
              local_species_list.push_back(species);
            }

          }
          // Finding the operators and assign value of -1.0 to "-" sign
          else if (term == "+" || term == "=" || term == "-")
          {
            if (term == "-")
            {
              sign = -1;
              term = "+";
            }
            else
              sign = 1;

            if (term == "=")
              secondary = true;
          }
          else
            mooseError("Error parsing term: ", term.as_string());
        }

        stos.push_back(local_stos);
        primary_aqueous_species_involved.push_back(local_species_list);
        // some useful screen out....
        // mooseAssert(stos[n_reactions-1].size() == primary_aqueous_species_involved[n_reactions-1], "aqueous parser not works ");
        _console << n_reactions<< "-th aqueous speciation reactions: ";
        for (unsigned int i = 0; i < stos[n_reactions-1].size(); ++i)
        {
          if (i < stos[n_reactions-1].size()-1)
            _console << "(" <<stos[n_reactions-1][i] << ")" <<primary_aqueous_species_involved[n_reactions-1][i]<< " + ";
          else
            _console << "(" <<stos[n_reactions-1][i] << ")" <<primary_aqueous_species_involved[n_reactions-1][i]<< "   ";
        }

        _console <<"<=> (1) "<<eq_species[n_reactions-1]<< "   log10_K="<<eq_const[n_reactions-1]<<std::endl;
      } // End parsing


      _console << "---------------------------------------------------------------------------------"<<std::endl;
      _console<<std::endl;

      if (n_reactions == 0)
        mooseWarning("No equilibrium aqueous speciation reaction provided!");
      else
      {
        // Start picking out primary species and coupled primary species and assigning corresponding stoichiomentric coefficients
        for (unsigned int i = 0; i < nl_vars.size(); ++i)
        {
          sto_u[i].resize(n_reactions);
          sto_v[i].resize(n_reactions);
          coupled_v[i].resize(n_reactions);
          weight[i].resize(n_reactions);

          primary_participation[i].resize(n_reactions, false);
          for (unsigned int j = 0; j < n_reactions; ++j)
          {
            for (unsigned int k = 0; k < primary_aqueous_species_involved[j].size(); ++k)
              if (primary_aqueous_species_involved[j][k] == nl_vars[i])
                primary_participation[i][j] = true;

            if (primary_participation[i][j])
              for (unsigned int k=0; k < primary_aqueous_species_involved[j].size(); k++)
              {
                if (primary_aqueous_species_involved[j][k] == nl_vars[i])
                {
                  sto_u[i][j] = stos[j][k];
                  weight[i][j] = stos[j][k];  //this term needs double check
                }
                else
                {
                  sto_v[i][j].push_back(stos[j][k]);
                  coupled_v[i][j].push_back(primary_aqueous_species_involved[j][k]);
                }
              }
          }
        }
      }

      for (unsigned int i = 0; i < nl_vars.size(); ++i)
      {
        //  Adding the coupled kernels if the primary species participates in this equilibrium reaction
        for (unsigned int j=0; j < eq_const.size(); j++)
          if (primary_participation[i][j])
          {
            // Building kernels for equilbirium aqueous species
            InputParameters params_sub = _factory.getValidParams("SecondaryAqueousSpeciesTimeIntegration");
            params_sub.set<NonlinearVariableName>("variable") = nl_vars[i];
            params_sub.set<Real>("weight") = weight[i][j];

            params_sub.set<Real>("log_k")  = eq_const[j];
            params_sub.set<Real>("sto_u")  = sto_u[i][j];
            params_sub.set<std::vector<Real> >("sto_v") = sto_v[i][j];
            params_sub.set<std::vector<VariableName> >("v") = coupled_v[i][j];
            _problem->addKernel("SecondaryAqueousSpeciesTimeIntegration", nl_vars[i]+"_"+eq_species[j]+"_sub", params_sub);

            InputParameters params_cd = _factory.getValidParams("SecondaryAqueousSpeciesDiffusion");
            params_cd.set<NonlinearVariableName>("variable") = nl_vars[i];
            params_cd.set<Real>("weight") = weight[i][j];
            params_cd.set<Real>("log_k") = eq_const[j];
            params_cd.set<Real>("sto_u") = sto_u[i][j];
            params_cd.set<std::vector<Real> >("sto_v") = sto_v[i][j];
            params_cd.set<std::vector<VariableName> >("v") = coupled_v[i][j];
            _problem->addKernel("SecondaryAqueousSpeciesDiffusion", nl_vars[i]+"_"+eq_species[j]+"_cd", params_cd);
          }
      }
    }
  } // End of setting up aqueous speciation reactions

  //
  // Setup mineral-solution kinetical precipitation/dissolution reactions
  //
  else if (_current_task == "add_minerals_aux_vars")
  {
    if (_pars.isParamValid("minerals"))
    {
      _console << std::endl;
      _console << "*********************************************************************************"<<std::endl;
      _console << "minerals reacting with aqueous solution: ";

      for (unsigned int i = 0; i < mineral_vars.size(); ++i)
      {
        _console << mineral_vars[i] << "\t";
        _problem->addAuxVariable(mineral_vars[i], _fe_type);
      }

      _console << std::endl;
      _console << "---------------------------------------------------------------------------------"<<std::endl;
    }
  }

  else if (_current_task == "add_minerals_initial_conc")
  {
    std::vector<Real> initial_mineral_vals = getParam<std::vector<Real> >("initial_mineral_conc");
    mooseAssert(initial_mineral_vals.size() == mineral_vars.size(), "expected intitial values assigned to each mineral");
    _console << std::endl;
    _console << "*********************************************************************************"<<std::endl;
    _console << "Initial mineral concentrationss: ";

    for (unsigned int i = 0; i < mineral_vars.size(); ++i)
    {
      _console << mineral_vars[i] << "= "<<initial_mineral_vals[i] << "\t";
      InputParameters params = _factory.getValidParams("ConstantIC");
      params.set<VariableName>("variable") = mineral_vars[i];
      params.set<Real>("value") = initial_mineral_vals[i];
      _problem->addInitialCondition("ConstantIC", "ic_for" + mineral_vars[i], params);
    }

    _console<<std::endl;
    _console << "---------------------------------------------------------------------------------"<<std::endl;
  }

  else if (_current_task == "add_minerals_kernels")
  {
    if (_pars.isParamValid("minerals"))
    {

     std::vector<std::string> mineral_reactions = getParam<std::vector<std::string> >("mineral_solution_reactions");
     if (aux_vars.size() == 0)
       mooseWarning("No equilibrium aqueous species provided!");

     for (unsigned int i = 0; i < nl_vars.size(); ++i)
     {
       std::vector<bool> primary_participation(mineral_reactions.size(), false);
       std::vector<std::string> solid_kin_species(mineral_reactions.size());


       for (unsigned int j = 0; j < mineral_reactions.size(); ++j)
       {
         std::vector<std::string> tokens;

         // Parsing each reaction
         MooseUtils::tokenize(mineral_reactions[j], tokens, 1, "+=");

         std::vector<Real> stos_coeff(tokens.size()-1);
         std::vector<std::string> rxn_vars(tokens.size()-1);

         for (unsigned int k=0; k < tokens.size(); k++)
         {
           std::vector<std::string> stos_vars;
           MooseUtils::tokenize(tokens[k], stos_vars, 1, "()");
           if (stos_vars.size() == 2)
           {
             Real coef;
             std::istringstream iss(stos_vars[0]);
             iss >> coef;
             stos_coeff[k] = coef;
             rxn_vars[k] = stos_vars[1];
             // Check the participation of primary species
             if (rxn_vars[k] == nl_vars[i]) primary_participation[j] = true;
           }
           else
             solid_kin_species[j] = stos_vars[0];
         }
        // Done parsing, recorded stochiometric and variables into separate arrays

         if (primary_participation[j])
         {
           std::vector<Real> mineral_weight(1);

           // Assigning the stochiometrics based on parsing
           for (unsigned int m = 0; m < rxn_vars.size(); ++m)
             if (rxn_vars[m] == nl_vars[i])
               mineral_weight[0] = stos_coeff[m];

           std::vector<VariableName> coupled_var(1);
           coupled_var[0] = solid_kin_species[j];
           _console << "Weight of mineral " << coupled_var[0] << " to primary sepcies "+nl_vars[i] << " = "<< mineral_weight[0] << "\n";

           // Building kernels for mineral-dissolution/precipitation
           InputParameters params_kin = _factory.getValidParams("MineralSolutionTimeIntegration");
           params_kin.set<NonlinearVariableName>("variable") = nl_vars[i];
           params_kin.set<std::vector<VariableName> >("mineral_compositions") = coupled_var;
           params_kin.set<std::vector<Real> >("sto_v") = mineral_weight;

           _problem->addKernel("MineralSolutionTimeIntegration", nl_vars[i] + "_" + solid_kin_species[j] + "_kin", params_kin);
         }
       }
     }
    }
  }
  else if (_current_task == "add_minerals_auxkernels")
  {
    if (_pars.isParamValid("minerals"))
    {
      std::vector<std::string> mineral_reactions = getParam<std::vector<std::string> >("mineral_solution_reactions");
      std::vector<Real> logk = getParam<std::vector<Real> >("log_k");
      std::vector<Real> r_area = getParam<std::vector<Real> >("specific_reactive_surface_area");
      std::vector<Real> ref_kconst = getParam<std::vector<Real> >("kinetic_rate_constant");
      std::vector<Real> e_act = getParam<std::vector<Real> >("activation_energy");
      Real gas_const = getParam<Real>("gas_constant");
      Real ref_temp = getParam<Real>("reference_temperature");
      Real sys_temp = getParam<Real>("system_temperature");

      // NEED TO ADD AN ERROR MESSAGE IF THE SIZES OF ABOVE ARRAYS ARE NOT THE SAME //
      _console << std::endl;
      _console << "*********************************************************************************"<<std::endl;
      _console <<"Mineral-solution interactions reaction network"<<std::endl;

      for (unsigned int j=0; j < mineral_reactions.size(); j++)
      {
        std::vector<std::string> tokens;
        std::vector<std::string> solid_kin_species(mineral_reactions.size());

        // Parsing each reaction
        MooseUtils::tokenize(mineral_reactions[j], tokens, 1, "+=");

        std::vector<Real> stos_primary(tokens.size()-1);
        std::vector<VariableName> rxn_vars(tokens.size()-1);

        for (unsigned int k=0; k < tokens.size(); k++)
        {
          std::vector<std::string> stos_vars;
          MooseUtils::tokenize(tokens[k], stos_vars, 1, "()");
          if (stos_vars.size() == 2)
          {
            Real coef;
            std::istringstream iss(stos_vars[0]);
            iss >> coef;
            stos_primary[k] = coef;
            rxn_vars[k] = stos_vars[1];
          }
          else
            solid_kin_species[j] = stos_vars[0];
        }

        InputParameters params_kin = _factory.getValidParams("MineralDissolutionPrecipAux");
        params_kin.set<AuxVariableName>("variable") = solid_kin_species[j];
        params_kin.set<Real>("log_k") = logk[j];
        params_kin.set<Real>("reactive_surface_area") = r_area[j];
        params_kin.set<Real>("ref_kconst") = ref_kconst[j];
        params_kin.set<Real>("e_act") = e_act[j];
        params_kin.set<Real>("gas_const") = gas_const;
        params_kin.set<Real>("ref_temp") = ref_temp;
        params_kin.set<Real>("sys_temp") = sys_temp;
        params_kin.set<std::vector<Real> >("sto_v") = stos_primary;
        params_kin.set<std::vector<VariableName> >("aqueous_species") = rxn_vars;
        _problem->addAuxKernel("MineralDissolutionPrecipAux", "aux_"+solid_kin_species[j], params_kin);

        _console << solid_kin_species[j]<<"<==>";

        for (unsigned int k=0; k < stos_primary.size(); k++)
        {
          if (k < stos_primary.size() -1)
            _console <<"("<<stos_primary[k]<<")"<<rxn_vars[k]<<" + ";
          else
            _console <<"("<<stos_primary[k]<<")"<<rxn_vars[k]<<"   ";
        }

        _console << "  log10_K="<<logk[j]<<"  ref_k_const="<<ref_kconst[j]<<std::endl;
      }

      _console<<std::endl;
      _console << "---------------------------------------------------------------------------------"<<std::endl;
    }
  } // End of setting up mineral precipitation & dissolution reactions
}
