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

#include "PorousMediaBase.h"

// libMesh includes
#include "libmesh/quadrature.h"

template<>
InputParameters validParams<PorousMediaBase>()
{
  InputParameters params = validParams<Material>();

  //parameters for ion diffusion through concrete & solution-mineral reactions
  params.addParam<Real>("initial_diffusivity",  1.0e-9, "diffusivity of ions in medium, m^2/s");
  params.addParam<Real>("initial_porosity",     0.3,     "Initial porosity of medium");
  params.addParam<Real>("initial_permeability", 1.0e-12, "Initial permeability of medium in m^2");
  params.addParam<Real>("initial_storativity",  1.0e-5,  "The specific storage of porous media");

  params.addCoupledVar("mineral_compositions", "minerals involved in reactions");
  params.addParam<std::vector<Real> >("initial_mineral_conc", " mol/L of solution");
  params.addParam<std::vector<Real> >("mineral_molecular_weight", "molecular weight, g/mol");
  params.addParam<std::vector<Real> >("mineral_density", "The density of minerals, g/cm^3");

  //parameters for thermal properties calculations
  MooseEnum thermal_conductivity_model("CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004 KIM-2003", "CONSTANT");
  MooseEnum thermal_capacity_model("CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004", "CONSTANT");
  MooseEnum aggregate_type("Siliceous Carbonate", "Siliceous");

  params.addParam<MooseEnum>("thermal_conductivity_model", thermal_conductivity_model, "thermal conductivity models");
  params.addParam<MooseEnum>("thermal_capacity_model", thermal_capacity_model, "thermal capacity models");
  params.addParam<MooseEnum>("aggregate_type", aggregate_type, "siliceous or carbonate");

  params.addParam<Real>("ref_density_of_concrete",   2231.0,  "refernece density of porous media Kg/m^3");
  params.addParam<Real>("ref_specific_heat_of_concrete",  1100.0,  "reference specific heat of concrete J/Kg/0C");
  params.addParam<Real>("ref_thermal_conductivity_of_concrete",  3.0,  "reference thermal conductivity of concrete  W/m/0C");


  //parameters for mositure capacity (dW/dH) and humidity diffusivity calculations
  MooseEnum cement_type("1 2 3 4", "1");
  MooseEnum aggregate_pore_type("dense porous", "dense");
  params.addParam<MooseEnum>("cement_type", cement_type, "cement type input for moisture capacity calculations");
  params.addParam<MooseEnum>("aggregate_pore_type", aggregate_pore_type, "aggregate pore  structure");

  MooseEnum moisture_diffusivity_model("Bazant Xi Mensi", "Bazant");
  params.addParam<MooseEnum>("moisture_diffusivity_model", moisture_diffusivity_model, "moisture diffusivity models");

  params.addParam<Real>("cement_mass",     354.0, "cement mass (kg) per m^3");
  params.addParam<Real>("aggregate_mass", 1877.0, "aggregate mass (kg) per m^3");
  params.addParam<Real>("water_to_cement_ratio",  0.43, "water to cement ratio");
  params.addParam<Real>("concrete_cure_time", 23.0, "concrete curing time in days");
  params.addParam<Real>("aggregate_vol_fraction", 0.7, "volumetric fraction of aggregates");

  //parameters for Bazant mositure diffusivity model
  params.addParam<Real>("D1", 3.0e-10, "empirical constants");
  params.addParam<Real>("n", 6.0, "empirical constants");

  //parameters for Mensi's moisture diffusivity model
  params.addParam<Real>("A", 3.8e-13, "empirical constants");
  params.addParam<Real>("B", 0.05, "empirical constants");
  params.addParam<Real>("C0",130.0, "empirical constants");

  params.addParam<Real>("coupled_moisture_diffusivity_factor",  1.0e-5,  "coupling coefficient mositure transfer due to heat");


  params.addCoupledVar("relative_humidity", "nonlinear variable name for rel. humidity");
  params.addCoupledVar("temperature", "nonlinear variable name for temperature in unit of Celscius");

  return params;

}

PorousMediaBase::PorousMediaBase(const InputParameters & parameters)
  :Material(parameters),
   _thermal_conductivity_model(getParam<MooseEnum>("thermal_conductivity_model")),
   _thermal_capacity_model(getParam<MooseEnum>("thermal_capacity_model")),
   _aggregate_type(getParam<MooseEnum>("aggregate_type")),
   _cement_type(getParam<MooseEnum>("cement_type")),
   _aggregate_pore_type(getParam<MooseEnum>("aggregate_pore_type")),

   _moisture_diffusivity_model(getParam<MooseEnum>("moisture_diffusivity_model")),

   _input_initial_diffusivity(getParam<Real>("initial_diffusivity")),
   _input_initial_porosity(getParam<Real>("initial_porosity")),
   _input_initial_permeability(getParam<Real>("initial_permeability")),
   _input_initial_storativity(getParam<Real>("initial_storativity")),
   _input_bulk_density(getParam<Real>("ref_density_of_concrete")),

   _mineral_molecular_weight(getParam<std::vector<Real> >("mineral_molecular_weight")),
   _mineral_density(getParam<std::vector<Real> >("mineral_density")),
   _initial_mineral_conc(getParam<std::vector<Real> >("initial_mineral_conc")),

   _water_to_cement(getParam<Real>("water_to_cement_ratio")),
   _cure_time(getParam<Real>("concrete_cure_time")),

   _cement_mass(getParam<Real>("cement_mass")),
   _aggregate_mass(getParam<Real>("aggregate_mass")),

   _D1(getParam<Real>("D1")),
   _n_power(getParam<Real>("n")),

   _A(getParam<Real>("A")),
   _B(getParam<Real>("B")),
   _C0(getParam<Real>("C0")),

   _agg_vol_fraction(getParam<Real>("aggregate_vol_fraction")),

   _input_density_of_concrete(getParam<Real>("ref_density_of_concrete")),
   _input_specific_heat_of_concrete(getParam<Real>("ref_specific_heat_of_concrete")),
   _input_thermal_conductivity_of_concrete(getParam<Real>("ref_thermal_conductivity_of_concrete")),
   _alfa_Dht(getParam<Real>("coupled_moisture_diffusivity_factor")),

// some intrinsic material properties
   _diffusivity(declareProperty<Real>("diffusivity")),
   _permeability(declareProperty<Real>("permeability")),
   _storativity(declareProperty<Real>("storativity")),
   _porosity(declareProperty<Real>("porosity")),

//material properties asscociated with coupled mositure/thermal transfer through concrete
   _thermal_capacity(declareProperty<Real>("thermal_capacity")),
   _thermal_conductivity(declareProperty<Real>("thermal_conductivity")),
   _ca(declareProperty<Real>("heat_absorption_of_water")),
   _moisture_capacity(declareProperty<Real>("moisture_capacity")),
   _moisture_content(declareProperty<Real>("moisture_content")),
   _cw(declareProperty<Real>("thermal_capacity_of_water")),
   _darcy_moisture_flux(declareProperty<RealGradient>("darcy_moisture_flux")),
   _Dh(declareProperty<Real>("humidity_diffusivity")),
   _Dht(declareProperty<Real>("humidity_diffusivity_thermal")),

   _WH(declareProperty<Real>("hydrated_water")),
   _WH_old(declarePropertyOld<Real>("hydrated_water")),
   _te(declareProperty<Real>("eqv_hydrationtime")),
   _te_old(declarePropertyOld<Real>("eqv_hydrationtime")),
   _eqv_age(declareProperty<Real>("eqv_age")),
   _eqv_age_old(declarePropertyOld<Real>("eqv_age")),


   _has_rh(isCoupled("relative_humidity")),
   _rh(_has_rh ? coupledValue("relative_humidity") : _zero),
   _grad_rh(_has_rh ? coupledGradient("relative_humidity") : _grad_zero),


   _has_temperature(isCoupled("temperature")),
   _temp(_has_temperature ? coupledValue("temperature") : _zero)


{

  int n = coupledComponents("mineral_compositions");
  _vals.resize(n);
  for (unsigned int i=0; i<_vals.size(); ++i)
    _vals[i] = &coupledValue("mineral_compositions", i);
}

void
PorousMediaBase::initQpStatefulProperties()
{
  _te[_qp] = _cure_time;
  _eqv_age[_qp] = _cure_time;
  _WH[_qp] = 0.21 * _cement_mass * std::pow(_te[_qp]/(_cure_time +_te[_qp]),1.0/3);
}

void
PorousMediaBase::computeProperties()
{
  for(unsigned int qp=0; qp<_qrule->n_points(); ++qp)
  {

    // Initialize the material properties from input

    _porosity[qp] = _input_initial_porosity;
    _permeability[qp] = _input_initial_permeability;
    _diffusivity[qp]  = _input_initial_diffusivity;

    if (_vals.size() > 0) //porosity/permeability changes due to mineral precipitation or dissolution
    {
      Real _initial_mineral_vof = 0.0;
      Real _current_mineral_vof = 0.0;

      for (unsigned int i=0; i<_vals.size(); ++i)
      {
        _initial_mineral_vof += 1.0e-3 * _initial_mineral_conc[i]
                               * _mineral_molecular_weight[i]
                               / _mineral_density[i]
                               * _input_initial_porosity;

        _current_mineral_vof += 1.0e-3 * (*_vals[i])[qp]
                               * _mineral_molecular_weight[i]
                               / _mineral_density[i]
                               * _input_initial_porosity;
      }

      _porosity[qp] = _input_initial_porosity - (_current_mineral_vof - _initial_mineral_vof);

      //minimum porosity allowed
      if (_porosity[qp] < 1.0e-3) _porosity[qp]=1.0e-3;

      // Permeability changes calculated from porosity changes according to Power Law
      //  with an order of 5.2: k=ki* (n/ni)^5.2
      _permeability[qp] = _input_initial_permeability
                         * std::pow(_porosity[qp]/_input_initial_porosity, 5.2);

    }

    Real T = _temp[qp];
    Real H = _rh[qp];
    if (H < 0.0) H = 0.0;

// compute equivalent concrete age used in Xi's moisture capavity model
    _eqv_age[qp] = _eqv_age_old[qp] + _dt/86400.0 * 1.0/(1.0+std::pow((7.5-7.5*H),4.0));

// compute equivalent hydration time used in compute the amount of dehydrated water
    if (T < 105.0)
    {
      _te[qp] = _te_old[qp] + _dt/86400.0 * 1.0/(1.0+std::pow((3.5-3.5*H),4.0))
                              * std::exp(2700.0*(1.0/(273.15+25.0)-1.0/(273.15+    T)));
    }
    else
    {
      _te[qp] = _te_old[qp] + _dt/86400.0 * 1.0/(1.0+std::pow((3.5-3.5*H),4.0))
                              * std::exp(2700.0*(1.0/(273.15+25.0)-1.0/(273.15+105.0)));
    }

// compute amount of hydrated water
    _WH[qp] = 0.21 * _cement_mass * std::pow(_te[qp]/(_cure_time +_te[qp]),1.0/3);


    Real ro = _input_density_of_concrete;
    Real Cv = _input_specific_heat_of_concrete;
// compute concrete thermal capacity
    switch (_thermal_capacity_model)
    {
    case 0:  //CONSTANT
      _thermal_capacity[qp] = ro * Cv;
      break;
    case 1:  //ASCE-1992 for normal strength concrete
      switch (_aggregate_type)
      {
      case 0: //siliceous aggreagte
        if(T < 20.0)
        {
          _thermal_capacity[qp] = ro * Cv;
        }
        else if (T >= 20.0 && T < 200.0)
        {
          _thermal_capacity[qp] = (0.005 * T + 1.7) * 1.0e6;
        }
        else if (T >= 200.0 && T < 400.0)
        {
          _thermal_capacity[qp] = 2.7 * 1.0e6;
        }
        else if (T >= 400.0 && T < 500.0)
        {
          _thermal_capacity[qp] = (0.013 * T - 2.5) * 1.0e6;
        }
        else if (T >= 500.0 && T < 600.0)
        {
          _thermal_capacity[qp] = (10.5 - 0.013 * T) * 1.0e6;
        }
        else if (T >= 600.0)
        {
          _thermal_capacity[qp] = 2.7 * 1.0e6;
        }
        break;

      case 1: //carbonate aggregate
        if(T < 20.0)
        {
          _thermal_capacity[qp] = ro * Cv;
        }
        else if (T >= 20.0 && T < 400.0)
        {
          _thermal_capacity[qp] = 2.566  * 1.0e6;
        }
        else if (T >= 400.0 && T < 410.0)
        {
          _thermal_capacity[qp] = (0.1765 * T - 68.034)  * 1.0e6;
        }
        else if (T >= 410.0 && T < 445.0)
        {
          _thermal_capacity[qp] = (25.00671 - 0.05043 * T)  * 1.0e6;
        }
        else if (T >= 445.0 && T < 500.0)
        {
          _thermal_capacity[qp] = 2.566 * 1.0e6;
        }
        else if (T >= 500.0 && T < 635.0)
        {
          _thermal_capacity[qp] = (0.01603 * T - 5.44881)  * 1.0e6;
        }
        else if (T >= 635.0 && T < 715.0)
        {
          _thermal_capacity[qp] = (0.16635 * T - 100.90225) * 1.0e6;
        }
        else if (T >= 715.0 && T < 785.0)
        {
          _thermal_capacity[qp] = (176.07343 - 0.22103 * T)  * 1.0e6;
        }
        else if (T >= 785.0)
        {
          _thermal_capacity[qp] = 2.566  * 1.0e6;
        }
        break;

      default:mooseError("Unknown aggregate types");
        break;
      }
      break;

    case 2: // KODUR-2004  for high strength concrere
      switch (_aggregate_type)
      {
      case 0: //siliceous aggreagte
        if(T < 20.0)
        {
          _thermal_capacity[qp] = ro * Cv;
        }
        else if (T >= 20.0 && T < 200.0)
        {
          _thermal_capacity[qp] = (0.005 * T + 1.7) * 1.0e6;
        }
        else if (T >= 200.0 && T < 400.0)
        {
          _thermal_capacity[qp] = 2.7 * 1.0e6;
        }
        else if (T >= 400.0 && T < 500.0)
        {
          _thermal_capacity[qp] = (0.013 * T - 2.5) * 1.0e6;
        }
        else if (T >= 500.0 && T < 600.0)
        {
          _thermal_capacity[qp] = (10.5 - 0.013 * T) * 1.0e6;
        }
        else if (T >= 600.0 && T < 635.0)
        {
          _thermal_capacity[qp] = 2.7 * 1.0e6;
        }
        else if (T > 635.0)
        {
          mooseError("Temperature outside of the range for the selected thermal capacity model");
        }

        break;

      case 1: //carbonate aggregate

        if(T < 20.0)
        {
          _thermal_capacity[qp] = ro * Cv;
        }
        else if (T >= 20.0 && T < 400.0)
        {
          _thermal_capacity[qp] = 2.45  * 1.0e6;
        }
        else if (T >= 400.0 && T < 475.0)
        {
          _thermal_capacity[qp] = (0.026 * T - 12.85)  * 1.0e6;
        }
        else if (T >= 475.0 && T < 650.0)
        {
          _thermal_capacity[qp] = (0.0143 * T - 6.295)  * 1.0e6;
        }
        else if (T >= 650.0 && T < 735.0)
        {
          _thermal_capacity[qp] = (0.1894 * T - 120.11) * 1.0e6;
        }
        else if (T >= 735.0 && T < 800.0)
        {
          _thermal_capacity[qp] = (-0.263 * T + 212.4)  * 1.0e6;
        }
        else if (T >= 800.0 && T <= 1000.0)
        {
          _thermal_capacity[qp] = 2.00  * 1.0e6;
        }
        else if (T > 1000.0)
        {
          mooseError("Temperature outside of the range for the selected thermal capacity model");
        }

        break;

      default:mooseError("Unknown aggregate types");
        break;
      }
      break;

    case 3: //EUROCODE-2004 for both normal ans high strength concrete
      // compute density of concrete


      if (T < 115.0)
      {
        ro = _input_density_of_concrete;
      }
      else if (T >= 115.0 && T < 200.0)
      {
        ro = _input_density_of_concrete * (1.0 - 0.02*(T-115.0)/85.0);
      }
      else if (T >= 200.0 && T < 400.0)
      {
        ro = _input_density_of_concrete * (0.98 - 0.03*(T-200.0)/200.0);
      }
      else if (T >= 400.0 && T < 1200.0)
      {
        ro = _input_density_of_concrete * (0.95 - 0.07*(T-400.0)/800.0);
      }
      else if (T >= 1200.0)
      {
        mooseError("Temperature outside of the range for the selected thermal capacity model");
      }

      if (T < 100.0)
      {
        Cv = 900.0;
      }
      else if (T >= 100.0 && T < 200.0)
      {
        Cv =  900.0 + (T - 100.0);
      }
      else if (T >= 200.0 && T < 400.0)
      {
        Cv = 1000.0 + (T - 200.0)/2.0;
      }
      else if (T >= 400.0 && T < 1200.0)
      {
        Cv = 1100.0;
      }
      else if (T >= 1200.0)
      {
        mooseError("Temperature outside of the range for the selected thermal capacity model");
      }

      _thermal_capacity[qp] = ro  * Cv;

      break;

    default: mooseError("Unknown thermal capacity model");
      break;
    }

// compute thermal conductivity
    Real k_up = 0.0;
    Real k_low = 0.0;

    switch (_thermal_conductivity_model)
    {
    case 0:  //CONSTANT
      _thermal_conductivity[qp] = _input_thermal_conductivity_of_concrete;
      break;
    case 1: //ASCE-1992
      switch (_aggregate_type)
      {
      case 0: //siliceous aggreagte
        if(T < 20.0)
        {
          _thermal_conductivity[qp] = _input_thermal_conductivity_of_concrete;
        }
        else if (T >= 20.0 && T < 800.0)
        {
          _thermal_conductivity[qp] = -0.000625 * T + 1.5;
        }
        else if (T >= 800.0)
        {
          _thermal_conductivity[qp] = 1.0;
        }
        break;

      case 1: //carbonate aggregate
        if(T < 20.0)
        {
          _thermal_conductivity[qp] = _input_thermal_conductivity_of_concrete;
        }
        else if (T >= 20.0 && T < 293.0)
        {
          _thermal_conductivity[qp] = 1.355;
        }
        else if (T >= 293.0)
        {
          _thermal_conductivity[qp] = -0.001241 * T + 1.7162;
        }
        break;

      default:mooseError("Unknown aggregate types");
        break;
      }
      break;

    case 2: //KODUR-2004
      switch (_aggregate_type)
      {
      case 0: //siliceous aggreagte
        if(T < 20.0)
        {
          _thermal_conductivity[qp] = _input_thermal_conductivity_of_concrete;
        }
        else if (T >= 20.0 && T < 800.0)
        {
          _thermal_conductivity[qp] = -0.000625 * T + 1.5;
        }
        else if (T >= 800.0)
        {
          _thermal_conductivity[qp] = 1.0;
        }
        break;

      case 1: //carbonate aggregate
        if(T < 20.0)
        {
          _thermal_conductivity[qp] = _input_thermal_conductivity_of_concrete;
        }
        else if (T >= 20.0 && T < 293.0)
        {
          _thermal_conductivity[qp] = 1.355;
        }
        else if (T >= 293.0)
        {
          _thermal_conductivity[qp] = -0.001241 * T + 1.7162;
        }
        break;

      default:mooseError("Unknown aggregate types");
        break;
      }
      break;
    case 3: //EUROCODE-2004
      if(T < 20.0)
      {
        _thermal_conductivity[qp] = _input_thermal_conductivity_of_concrete;
      }
      else if (T >= 20.0 && T <= 1200.0)
      {
        k_up = 2.0  - 0.2451*(T/100.0)+0.0107*std::pow(T/100.0, 2.0);
        k_low= 1.36 -  0.136*(T/100.0)+0.0057*std::pow(T/100.0, 2.0);
        _thermal_conductivity[qp] = (k_up + k_low)/2.0;  //average bewteen upper and lower bounds
      }
      else if (T>1200.0)
      {
        mooseError("Temperature outside of the range for the selected thermal conductivity model");
      }
      break;

    case 4: //KIM-2004
      mooseError("KIM thermal conductivity model not implemented yet");
      break;
    default:
      mooseError("Unknown thermal conductivity model");
      break;
    }

//compute heat absorption and thermal capacity of water



    Real Clatent = 900.0;

    if (T <= 100.0)
    {
      Clatent = 900.0;
    }
    else if (T>100.0 && T<=200.0)
    {
      Clatent = 900.0 + (T-100.0);
    }
    else if (T>200.0 && T<=400.0)
    {
      Clatent = 1000.0 + (T -200.0)/2.0;
    }
    else if (T>400.0 && T<=1200.0)
    {
      Clatent = 1100.0;
    }
    else
    {
      Clatent = 1100.0;
    }
    //adsorption heat of water vapor
    _ca[qp] = 0.001 * Clatent;
    //thermal_capacity of water
    _cw[qp] = 350000.0 * std::pow(374.15 - T, 1.0/3.0);

// compute mositure capacity dW/dH
    //compute mositure capacity dw/dH for cement
    Real C = std::exp(855.0/(T+273.15));

    Real N_ct = 1.1;
    switch (_cement_type)
    {
    case 0:
      N_ct = 1.1;
      break;
    case 1:
      N_ct = 1.0;
      break;
    case 2:
      N_ct = 1.15;
      break;
    case 3:
      N_ct = 1.5;
      break;
    default: mooseError("Unknown cement type in mositure capacity calculations");
      break;
    }

    Real N_wc = 0.9;
    if (_water_to_cement < 0.3)
    {
      N_wc = 0.9;
    }
    else if (_water_to_cement >= 0.3 && _water_to_cement <= 0.7)
    {
      N_wc = 0.33 + 2.2 * _water_to_cement;
    }
    else
    {
      N_wc = 1.87;
    }

    Real N_te = 5.5;
    if (_eqv_age[qp] >= 5)
      N_te = 2.5 + 15.0 / _eqv_age[qp];

    Real n = N_te * N_wc * N_ct * 1.0;

    Real k = ((1.0-1.0/n)*C - 1.0)/(C-1.0);
    if (k < 0.0)
      k = 0.0;
    else if (k > 1.0)
      k = 1.0;

    Real V_ct = 0.9;
    switch (_cement_type)
    {
    case 0:
      V_ct = 0.9;
      break;
    case 1:
      V_ct = 1.0;
      break;
    case 2:
      V_ct = 0.85;
      break;
    case 3:
      V_ct = 0.6;
      break;
    default: mooseError("Unknown cement type in mositure capacity calculations");
      break;
    }

    Real V_wc = 0.985;
    if (_water_to_cement < 0.3)
    {
      V_wc = 0.985;
    }
    else if (_water_to_cement >= 0.3 && _water_to_cement <= 0.7)
    {
      V_wc = 0.85 + 0.45 * _water_to_cement;
    }
    else
    {
      V_wc = 1.165;
    }

    Real V_te = 0.024;
    if (_eqv_age[qp] >= 5)
      V_te = 0.068 - 0.22 / _eqv_age[qp];

    Real Vm = V_te * V_wc * V_ct * 1.0;

    Real W_cement = C*k*Vm*H/(1.0-k*H)/(1.0+(C-1.0)*k*H);

    Real dWdH_cement = (C*k*Vm + W_cement * k*(1.0+(C-1.0)*k*H) - W_cement*k*(1.0-k*H)*(C-1.0))
               /(1.0-k*H)/(1.0+(C-1.0)*k*H);

    //compute mositure capacity dw/dH for aggregates

    Real n_agg = 1.5;

    switch(_aggregate_pore_type)
    {
    case 0:
      n_agg = 1.5;
      break;
    case 1:
      n_agg = 2.0;
      break;
    default:  mooseError("unknown aggregate pore sturtures");
      break;
    }

    n = 4.063* n_agg;
    k = ((1.0-1.0/n)*C - 1.0)/(C-1.0);
    if (k < 0.0)
      k = 0.0;
    else if (k > 1.0)
      k = 1.0;

    Real V_agg = 0.05;
    switch(_aggregate_pore_type)
    {
    case 0:
      V_agg = 0.05;
      break;
    case 1:
      V_agg = 0.10;
      break;
    default:  mooseError("unknown aggregate pore sturtures");
      break;
    }

    Vm = 0.0647*V_agg;

    Real W_agg = C*k*Vm*H/(1.0-k*H)/(1.0+(C-1.0)*k*H);
    Real dWdH_agg = (C*k*Vm + W_agg * k*(1.0+(C-1.0)*k*H) - W_agg*k*(1.0-k*H)*(C-1.0))
               /(1.0-k*H)/(1.0+(C-1.0)*k*H);


// compute weight percentages of agrregates and cement paste
    Real _f_agg = _aggregate_mass/(_aggregate_mass + _cement_mass);
    Real _f_cp  = _cement_mass   /(_aggregate_mass + _cement_mass);
//    Real _f_agg = _agg_vol_fraction * 2.5/2.3;
//    Real _f_cp  = (1.0 - _agg_vol_fraction) * 2.5/2.3;

//compute combined dW/dH of concrete
    _moisture_capacity[qp] = ( _f_agg * dWdH_agg + _f_cp * dWdH_cement) * _input_density_of_concrete;
    _moisture_content[qp]  = ( _f_agg * W_agg + _f_cp * W_cement) * _input_density_of_concrete;

//    _moisture_capacity[qp] = _f_agg * dWdH_agg + _f_cp * dWdH_cement;

//    _moisture_content[qp]  = _f_agg * W_agg    + _f_cp * W_cement;


// compute moisture diffusivity

    Real Dh0 = 3.10e-10;

    Real f1_T = 0.0;

    if (T <= 95.0)
      f1_T = std::exp(2700.0*(1.0/(25.0+273.15)-1.0/(   T+273.15)));
    else
      f1_T = std::exp(2700.0*(1.0/(25.0+273.15)-1.0/(95.0+273.15)));

    Real f2_T = 0.0;
    if (T > 95.0)
      f2_T = std::exp((T-95.0)/(0.881+0.214*(T-95.0)));

// parameters associated with Bazant model
    Real alfa_d = 1.0;
    if (T <= 95.0)
      alfa_d = 1.0/(1.0+19.0*(95.0-T)/70.0);
    else
      alfa_d = 1.0;

// Parameters associated with Mensi's model
    Real A = _A;
    Real B = _B;
    Real C0= _C0;
    Real C1 = H * C0;

// parameters associated with Xi's model-something's wrong for this model
    Real gi = _agg_vol_fraction;
    Real wc = _water_to_cement;

    Real alfa_h  =  1.05 -   3.8 * wc + 3.56  * std::pow(wc, 2.0);
    Real betta_h = -14.4 +  50.4 * wc - 41.8  * std::pow(wc, 2.0);
    Real gamma_h =  31.3 - 136.0 * wc + 162.0 * std::pow(wc, 2.0);

    Real power1 = std::pow(10.0, gamma_h*(H-1));
    Real power2 = std::pow( 2.0, -1.0 * power1);

    Real Dhcp = alfa_h + betta_h * (1.0 - power2);



    switch(_moisture_diffusivity_model)
    {
    case 0: //Bazant
      if (T <= 95.0)
        Dh0 = _D1 * ( alfa_d + (1-alfa_d)/(1.0+std::pow((1.0-H)/(1.0-0.75), _n_power)) );
      else
        Dh0 = _D1;
      break;
    case 1: //Xi
      Dh0 = Dhcp * (1+ gi/((1-gi)/3-1));
      break;
    case 2: //Mensi
      Dh0 = A * std::exp(B*C1);
      break;
    default:  mooseError("unknown moisture diffusivity models");
      break;
    }

//    _console << alfa_h<<","<<betta_h<<","<<gamma_h<<","<<Dhcp <<","<<Dh0<<std::endl;
    //compute the mosisture diffusivity
//    Dh0 = _D1;

    if (T <= 95.0)
      _Dh[qp] = Dh0 * f1_T;
    else
      _Dh[qp] = Dh0 * f1_T * f2_T;


    //compute the coupled mositure diffusivity due to thermal gradient
    _Dht[qp] = _alfa_Dht * _Dh[qp];

    _darcy_moisture_flux[qp] = - _Dh[qp] * _grad_rh[qp];


    }
}
