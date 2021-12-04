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

#include "ConcreteThermalMoisture.h"
#include "MooseObjectName.h"

// libMesh includes
#include "libmesh/quadrature.h"

registerMooseObject("BlackBearApp", ConcreteThermalMoisture);

InputParameters
ConcreteThermalMoisture::validParams()
{
  InputParameters params = Material::validParams();
  params.addRequiredParam<std::string>(
      "type", "A string representing the Moose Object that is used to call this class");

  // // parameters which will be deprecated soon
  MooseEnum thermal_capacity_model("CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004");
  MooseEnum thermal_conductivity_model("CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004 KIM-2003");
  params.addDeprecatedParam<MooseEnum>("thermal_capacity_model",
                                       thermal_capacity_model,
                                       "thermal capacity models",
                                       "Use thermal_model instead");
  params.addDeprecatedParam<MooseEnum>("thermal_conductivity_model",
                                       thermal_conductivity_model,
                                       "thermal conductivity models",
                                       "Use thermal_model instead");
  MooseEnum moisture_diffusivity_model("Bazant Xi Mensi");
  params.addDeprecatedParam<MooseEnum>("moisture_diffusivity_model",
                                       moisture_diffusivity_model,
                                       "moisture diffusivity models",
                                       "Use moisture_model instead");
  params.addDeprecatedParam<Real>("ref_density_of_concrete",
                                  "refernece density of porous media Kg/m^3",
                                  "Use ref_density instead");
  params.addDeprecatedParam<Real>("ref_specific_heat_of_concrete",
                                  "reference specific heat of concrete J/Kg/0C",
                                  "Use ref_specific_heat instead");
  params.addDeprecatedParam<Real>("ref_thermal_conductivity_of_concrete",
                                  "reference thermal conductivity of concrete  W/m/0C",
                                  "Use ref_thermal_conductivity instead");

  // available model for thermal and moisture transport
  MooseEnum thermal_model("CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004");
  params.addParam<MooseEnum>(
      "thermal_model", thermal_model, "Model for properties used in thermal");
  MooseEnum moisture_model("Bazant Mensi Xi");
  params.addParam<MooseEnum>(
      "moisture_model", moisture_model, "Model for properties used in moisture transport");

  // concrete properties
  MooseEnum cement_type("1 2 3 4");
  MooseEnum aggregate_type("Siliceous Carbonate");
  MooseEnum aggregate_pore_type("dense porous");
  params.addParam<MooseEnum>(
      "cement_type", cement_type, "cement type input for moisture capacity calculations");
  params.addParam<MooseEnum>("aggregate_type", aggregate_type, "Type of aggregate");
  params.addParam<MooseEnum>(
      "aggregate_pore_type", aggregate_pore_type, "aggregate pore structure");

  params.addParam<Real>("cement_mass", "cement mass (kg) per m^3");
  params.addParam<Real>("aggregate_mass", "aggregate mass (kg) per m^3");
  params.addParam<Real>("water_to_cement_ratio", "water to cement ratio");
  params.addParam<Real>("aggregate_vol_fraction", "volumetric fraction of aggregates");
  params.addParam<Real>("concrete_cure_time", "concrete curing time in days");
  params.addParam<Real>("ref_density", "refernece density of porous media Kg/m^3");
  params.addParam<Real>("ref_specific_heat", "reference specific heat of concrete J/Kg/0C");
  params.addParam<Real>("ref_thermal_conductivity",
                        "concrete reference thermal conductivity (W/m/C)");

  // parameters for Bazant mositure transport model
  params.addParam<Real>("D1", "empirical constants (m2/s)");
  params.addParam<Real>("n", "empirical constants");
  params.addParam<Real>("critical_relative_humidity", "empirical constants");
  params.addParam<Real>("coupled_moisture_diffusivity_factor",
                        "coupling coefficient mositure transfer due to heat");

  // parameters for Mensi's moisture model
  params.addParam<Real>("A", "empirical constants (m2/s)");
  params.addParam<Real>("B", "empirical constants");

  params.addCoupledVar("relative_humidity", "nonlinear variable name for rel. humidity");
  params.addCoupledVar("temperature",
                       "nonlinear variable name for temperature in unit of Celscius");
  params.addClassDescription("Material parameters for thermal and moisture transport in concrete.");

  return params;
}

ConcreteThermalMoisture::ConcreteThermalMoisture(const InputParameters & parameters)
  : Material(parameters),
    _thermal_model(getParam<MooseEnum>("thermal_model")),
    _moisture_model(getParam<MooseEnum>("moisture_model")),

    // concrete mix proportion paramters
    _cement_mass(isParamValid("cement_mass") ? getParam<Real>("cement_mass") : 0),
    _aggregate_mass(isParamValid("aggregate_mass") ? getParam<Real>("aggregate_mass") : 0),
    _water_to_cement(isParamValid("water_to_cement_ratio") ? getParam<Real>("water_to_cement_ratio")
                                                           : 0),

    // CONSTANT thermal trasport model parameters
    _input_density(isParamValid("ref_density") ? getParam<Real>("ref_density") : 0),
    _input_specific_heat(isParamValid("ref_specific_heat") ? getParam<Real>("ref_specific_heat")
                                                           : 0),
    _input_thermal_conductivity(
        isParamValid("ref_thermal_conductivity") ? getParam<Real>("ref_thermal_conductivity") : 0),

    // asce_1992 and kodur_2004 thermal model parameters
    _aggregate_type(getParam<MooseEnum>("aggregate_type")),

    // Bazant moisture model parameters
    _D1(isParamValid("D1") ? getParam<Real>("D1") : 0),
    _n_power(isParamValid("n") ? getParam<Real>("n") : 0),
    _Hc(isParamValid("critical_relative_humidity") ? getParam<Real>("critical_relative_humidity")
                                                   : 0),
    _alfa_Dht(isParamValid("coupled_moisture_diffusivity_factor")
                  ? getParam<Real>("coupled_moisture_diffusivity_factor")
                  : 0),

    // Mensi moisture model parameters
    _A(isParamValid("A") ? getParam<Real>("A") : 0),
    _B(isParamValid("B") ? getParam<Real>("B") : 0),

    // Xi moisture model parameters
    _cement_type(getParam<MooseEnum>("cement_type")),
    _aggregate_pore_type(getParam<MooseEnum>("aggregate_pore_type")),
    _agg_vol_fraction(
        isParamValid("aggregate_vol_fraction") ? getParam<Real>("aggregate_vol_fraction") : 0),
    _cure_time(isParamValid("concrete_cure_time") ? getParam<Real>("concrete_cure_time") : 0),

    // material properties asscociated with coupled mositure/thermal transfer through concrete
    _thermal_capacity(declareProperty<Real>("thermal_capacity")),
    _thermal_conductivity(declareProperty<Real>("thermal_conductivity")),
    _ca(declareProperty<Real>("heat_absorption_of_water")),
    _cw(declareProperty<Real>("thermal_capacity_of_water")),
    _moisture_capacity((_moisture_model == "Xi") ? &declareProperty<Real>("moisture_capacity")
                                                 : nullptr),

    _Dh(declareProperty<Real>("humidity_diffusivity")),
    _Dht(declareProperty<Real>("humidity_diffusivity_thermal")),
    _eqv_age(declareProperty<Real>("eqv_age")),
    _eqv_age_old(getMaterialPropertyOld<Real>("eqv_age")),

    _has_rh(isCoupled("relative_humidity")),
    _rh(_has_rh ? coupledValue("relative_humidity") : _zero),

    _has_temperature(isCoupled("temperature")),
    _temp(_has_temperature ? coupledValue("temperature") : _zero)
{
  Real parameter_error_check = true;
  if (parameters.isParamSetByUser("thermal_capacity_model"))
  {
    _thermal_model = getParam<MooseEnum>("thermal_capacity_model");
    _input_density = isParamValid("ref_density_of_concrete")
                         ? getParam<Real>("ref_density_of_concrete")
                         : 2231.0;
    _input_specific_heat = isParamValid("ref_specific_heat_of_concrete")
                               ? getParam<Real>("ref_specific_heat_of_concrete")
                               : 1100.0;
    _input_thermal_conductivity = isParamValid("ref_thermal_conductivity_of_concrete")
                                      ? getParam<Real>("ref_thermal_conductivity_of_concrete")
                                      : 3.0;
    _aggregate_type = isParamValid("aggregate_type") ? getParam<MooseEnum>("aggregate_type") : 0;
    parameter_error_check = false;
  }
  if (parameter_error_check)
  {
    if (_thermal_model == "CONSTANT")
    {
      if (!parameters.isParamSetByUser("ref_density"))
        mooseError("For CONSTANT thermal_model, ref_density must be defined.");
      if (!parameters.isParamSetByUser("ref_specific_heat"))
        mooseError("For constant thermal transport model, ref_specific_heat must be defined.");
      if (!parameters.isParamSetByUser("ref_thermal_conductivity"))
        mooseError(
            "For constant thermal transport model, ref_thermal_conductivity must be defined.");
    }
    else if (_thermal_model == "ASCE-1992")
    {
      if (!parameters.isParamSetByUser("aggregate_type"))
        mooseError("For ASCE-1992 thermal model, aggregate_type must be defined.");
    }
    else if (_thermal_model == "KODUR-2004")
    {
      if (!parameters.isParamSetByUser("aggregate_type"))
        mooseError("For KODUR-2004 thermal model, aggregate_type must be defined.");
    }
    else if (_thermal_model == "EUROCODE-2004")
    {
      if (!parameters.isParamSetByUser("ref_density"))
        mooseError("For EUROCODE-2004 thermal model, ref_density must be defined.");
      if (!parameters.isParamSetByUser("ref_specific_heat"))
        mooseError("For EUROCODE-2004 thermal model, ref_specific_heat must be defined.");
    }
  }

  parameter_error_check = true;

  if (parameters.isParamSetByUser("moisture_diffusivity_model"))
  {
    _moisture_model = getParam<MooseEnum>("moisture_diffusivity_model");
    _cement_mass = isParamValid("cement_mass") ? getParam<Real>("cement_mass") : 354.0;
    _water_to_cement =
        isParamValid("water_to_cement_ratio") ? getParam<Real>("water_to_cement_ratio") : 0.43;
    _cement_type = isParamValid("cement_type") ? getParam<MooseEnum>("cement_type") : 0;
    _aggregate_pore_type =
        isParamValid("aggregate_pore_type") ? getParam<MooseEnum>("aggregate_pore_type") : 0;
    _cure_time = isParamValid("concrete_cure_time") ? getParam<Real>("concrete_cure_time") : 23.0;
    _aggregate_mass = isParamValid("aggregate_mass") ? getParam<Real>("aggregate_mass") : 1877;
    _agg_vol_fraction =
        isParamValid("aggregate_vol_fraction") ? getParam<Real>("aggregate_vol_fraction") : 0.7;
    _D1 = isParamValid("D1") ? getParam<Real>("D1") : 3.0e-10;
    _Hc = isParamValid("critical_relative_humidity") ? getParam<Real>("critical_relative_humidity")
                                                     : 0.75;
    _n_power = isParamValid("n") ? getParam<Real>("n") : 6.0;
    _alfa_Dht = isParamValid("coupled_moisture_diffusivity_factor")
                    ? getParam<Real>("coupled_moisture_diffusivity_factor")
                    : 1.0e-5;
    _A = isParamValid("A") ? getParam<Real>("A") : 3.8e-13;
    _B = isParamValid("B") ? getParam<Real>("B") : 0.05;
    parameter_error_check = false;
  }

  if (parameter_error_check)
  {
    if (_moisture_model == "Bazant")
    {
      if (!parameters.isParamSetByUser("D1"))
        mooseError("For Bazant moisture model, empirical constant D1 must be defined.");
      if (!parameters.isParamSetByUser("n"))
        mooseError("For Bazant moisture model, empirical constant n must be defined.");
      if (!parameters.isParamSetByUser("critical_relative_humidity"))
        mooseError("For Bazant moisture model, critical_relative_humidity must be defined.");
      if (!parameters.isParamSetByUser("coupled_moisture_diffusivity_factor"))
        mooseError(
            "For Bazant moisture model, coupled_moisture_diffusivity_factor must be defined.");
    }
    else if (_moisture_model == "Mensi")
    {
      if (!parameters.isParamSetByUser("A"))
        mooseError("For Mensi moisture model, empirical constant A must be defined.");
      if (!parameters.isParamSetByUser("B"))
        mooseError("For Mensi moisture model, empirical constant B must be defined.");
      if (!parameters.isParamValid("cement_mass"))
        mooseError("For Mensi moisture model, cement_mass must be defined.");
      if (!parameters.isParamSetByUser("water_to_cement_ratio"))
        mooseError("For Mensi moisture model, water_to_cement_ratio must be defined.");
    }
    else if (_moisture_model == "Xi")
    {
      if (!parameters.isParamSetByUser("cement_type"))
        mooseError("For Xi moisture model, cement_type must be defined.");
      if (!parameters.isParamSetByUser("aggregate_pore_type"))
        mooseError("For Xi moisture model, aggregate_pore_type must be defined.");
      if (!parameters.isParamSetByUser("aggregate_vol_fraction"))
        mooseError("For Xi moisture model, aggregate_vol_fraction must be defined.");
      if (!parameters.isParamSetByUser("concrete_cure_time"))
        mooseError("For Xi moisture model, concrete_cure_time must be defined.");
      if (!parameters.isParamValid("cement_mass"))
        mooseError("For Xi moisture model, cement_mass must be defined.");
      if (!parameters.isParamSetByUser("aggregate_mass"))
        mooseError("For Xi moisture model, aggregate_mass must be defined.");
      if (!parameters.isParamSetByUser("water_to_cement_ratio"))
        mooseError("For Xi moisture model, water_to_cement_ratio must be defined.");
      if (_water_to_cement < 0.5)
        mooseError("For Xi's moisture model water_to_cement_ratio must be >= 0.5 to use Xi's model "
                   "for moisture diffusivity");
    }
  }
}

void
ConcreteThermalMoisture::initQpStatefulProperties()
{
  _eqv_age[_qp] = _cure_time;
}

void
ConcreteThermalMoisture::computeProperties()
{
  for (unsigned int qp = 0; qp < _qrule->n_points(); ++qp)
  {
    Real T = _temp[qp];
    Real H = _rh[qp];
    if (H < 0.0)
      H = 0.0;

    Real ro = _input_density;
    Real Cv = _input_specific_heat;

    // compute concrete thermal capacity
    switch (_thermal_model)
    {
      case 0: // CONSTANT
        _thermal_capacity[qp] = ro * Cv;
        _thermal_conductivity[qp] = _input_thermal_conductivity;
        break;
      case 1: // ASCE-1992 for normal strength concrete
        switch (_aggregate_type)
        {
          case 0: // siliceous aggreagte
            if (T < 20.0)
              _thermal_capacity[qp] = 1.8 * 1e6;
            else if (T >= 20.0 && T < 200.0)
              _thermal_capacity[qp] = (0.005 * T + 1.7) * 1.0e6;
            else if (T >= 200.0 && T < 400.0)
              _thermal_capacity[qp] = 2.7 * 1.0e6;
            else if (T >= 400.0 && T < 500.0)
              _thermal_capacity[qp] = (0.013 * T - 2.5) * 1.0e6;
            else if (T >= 500.0 && T < 600.0)
              _thermal_capacity[qp] = (10.5 - 0.013 * T) * 1.0e6;
            else if (T >= 600.0)
              _thermal_capacity[qp] = 2.7 * 1.0e6;

            if (T < 20.0)
              _thermal_conductivity[qp] = 1.4875;
            else if (T >= 20.0 && T < 800.0)
              _thermal_conductivity[qp] = -0.000625 * T + 1.5;
            else if (T >= 800.0)
              _thermal_conductivity[qp] = 1.0;
            break;

          case 1: // carbonate aggregate
            if (T < 400.0)
              _thermal_capacity[qp] = 2.566 * 1.0e6;
            else if (T >= 400.0 && T < 410.0)
              _thermal_capacity[qp] = (0.1765 * T - 68.034) * 1.0e6;
            else if (T >= 410.0 && T < 445.0)
              _thermal_capacity[qp] = (25.00671 - 0.05043 * T) * 1.0e6;
            else if (T >= 445.0 && T < 500.0)
              _thermal_capacity[qp] = 2.566 * 1.0e6;
            else if (T >= 500.0 && T < 635.0)
              _thermal_capacity[qp] = (0.01603 * T - 5.44881) * 1.0e6;
            else if (T >= 635.0 && T < 715.0)
              _thermal_capacity[qp] = (0.16635 * T - 100.90225) * 1.0e6;
            else if (T >= 715.0 && T < 785.0)
              _thermal_capacity[qp] = (176.07343 - 0.22103 * T) * 1.0e6;
            else if (T >= 785.0)
              _thermal_capacity[qp] = 2.566 * 1.0e6;

            if (T < 293.0)
              _thermal_conductivity[qp] = 1.355;
            else
              _thermal_conductivity[qp] = -0.001241 * T + 1.7162;
            break;

          default:
            mooseError("Unknown aggregate types");
            break;
        }
        break;

      case 2: // KODUR-2004  for high strength concrere
        switch (_aggregate_type)
        {
          case 0: // siliceous aggreagte
            if (T < 20.0)
              _thermal_capacity[qp] = 1.8 * 1.0e6;
            else if (T >= 20.0 && T < 200.0)
              _thermal_capacity[qp] = (0.005 * T + 1.7) * 1.0e6;
            else if (T >= 200.0 && T < 400.0)
              _thermal_capacity[qp] = 2.7 * 1.0e6;
            else if (T >= 400.0 && T < 500.0)
              _thermal_capacity[qp] = (0.013 * T - 2.5) * 1.0e6;
            else if (T >= 500.0 && T < 600.0)
              _thermal_capacity[qp] = (10.5 - 0.013 * T) * 1.0e6;
            else if (T >= 600.0 && T < 635.0)
              _thermal_capacity[qp] = 2.7 * 1.0e6;
            else if (T > 635.0)
              mooseError("Temperature outside of the range for the KODUR-2004 thermal model");

            if (T < 20.0)
              _thermal_conductivity[qp] = 1.4875;
            else if (T >= 20.0)
              _thermal_conductivity[qp] = -0.000625 * T + 1.5;
            break;

          case 1: // carbonate aggregate
            if (T < 400.0)
              _thermal_capacity[qp] = 2.45 * 1.0e6;
            else if (T >= 400.0 && T < 475.0)
              _thermal_capacity[qp] = (0.026 * T - 12.85) * 1.0e6;
            else if (T >= 475.0 && T < 650.0)
              _thermal_capacity[qp] = (0.0143 * T - 6.295) * 1.0e6;
            else if (T >= 650.0 && T < 735.0)
              _thermal_capacity[qp] = (0.1894 * T - 120.11) * 1.0e6;
            else if (T >= 735.0 && T < 800.0)
              _thermal_capacity[qp] = (-0.263 * T + 212.4) * 1.0e6;
            else if (T >= 800.0 && T <= 1000.0)
              _thermal_capacity[qp] = 2.00 * 1.0e6;
            else if (T > 1000.0)
              mooseError("Temperature outside of the range for the KODUR-2004 thermal model");

            if (T < 293.0)
              _thermal_conductivity[qp] = 1.355;
            else
              _thermal_conductivity[qp] = -0.001241 * T + 1.7162;
            break;

          default:
            mooseError("Unknown aggregate types");
            break;
        }
        break;

      case 3: // EUROCODE-2004 for both normal ans high strength concrete
        // compute density of concrete
        if (T < 115.0)
          ro = _input_density;
        else if (T >= 115.0 && T < 200.0)
          ro = _input_density * (1.0 - 0.02 * (T - 115.0) / 85.0);
        else if (T >= 200.0 && T < 400.0)
          ro = _input_density * (0.98 - 0.03 * (T - 200.0) / 200.0);
        else if (T >= 400.0 && T < 1200.0)
          ro = _input_density * (0.95 - 0.07 * (T - 400.0) / 800.0);
        else if (T >= 1200.0)
          mooseError("Temperature outside of the range for the EUROCODE-2004 thermal model");

        if (T < 100.0)
          Cv = 900.0;
        else if (T >= 100.0 && T < 200.0)
          Cv = 900.0 + (T - 100.0);
        else if (T >= 200.0 && T < 400.0)
          Cv = 1000.0 + (T - 200.0) / 2.0;
        else if (T >= 400.0 && T < 1200.0)
          Cv = 1100.0;

        _thermal_capacity[qp] = ro * Cv;

        if (T < 20.0)
          _thermal_conductivity[qp] = 1.642218;
        else if (T >= 20.0 && T <= 1200.0)
        {
          const Real k_up = 2.0 - 0.2451 * (T / 100.0) + 0.0107 * std::pow(T / 100.0, 2.0);
          const Real k_low = 1.36 - 0.136 * (T / 100.0) + 0.0057 * std::pow(T / 100.0, 2.0);
          _thermal_conductivity[qp] = (k_up + k_low) / 2.0; // average bewteen upper and lower
                                                            // bounds
        }
        break;

      default:
        mooseError("Unknown thermal model");
        break;
    }

    // compute moisture diffusivity
    switch (_moisture_model)
    {
      case 0: // Bazant
      {
        if (_pars.isParamSetByUser("moisture_model") && T < 25.0)
          mooseError("Temperature outside of the range for the Bazant moisture model");
        // adsorption heat of water vapor
        Real adsorption_heat_water =
            60 * 1000 / 0.018; // an average value (60 kJ/mol) estimated for different cement paste
                               // by Poyet and Charles (2009)
        Real density_water = 1000;       // kg/m3
        Real heat_capacity_water = 4180; // J/(kg-C)
        _ca[qp] = adsorption_heat_water;
        _cw[qp] = density_water * heat_capacity_water;

        if (T <= 95.0)
        {
          Real alfa_T = 0.05 + 0.95 / 70 * (T - 25); // Bazant and Thonguthai 1979
          Real f1_h =
              alfa_T + (1 - alfa_T) / (1.0 + std::pow(((1.0 - H) / (1 - _Hc)),
                                                      _n_power)); // Bazant and Thonguthai 1979
          Real f2_T = std::exp(
              2700.0 * (1.0 / (25.0 + 273.15) - 1.0 / (T + 273.15))); // Bazant and Thonguthai 1979
          _Dh[qp] = _D1 * f1_h * f2_T;
        }
        else
        {
          Real f2_95 = std::exp(
              2700.0 * (1.0 / (25.0 + 273.15) - 1.0 / (95 + 273.15))); // Bazant and Thonguthai 1979
          Real f3_T =
              std::exp((T - 95.0) / (0.881 + 0.214 * (T - 95.0))); // Bazant and Thonguthai 1979
          _Dh[qp] = _D1 * f2_95 * f3_T;                            // Bazant and Thonguthai 1979
        }
        // compute the coupled mositure diffusivity due to thermal gradient
        _Dht[qp] = _alfa_Dht * _Dh[qp];
        break;
      }

      case 1: // Mensi
      {
        Real initial_water_content = _water_to_cement * _cement_mass; // kg/m3
        _Dh[qp] = _A * std::exp(_B * initial_water_content);
        _ca[qp] = 0.;
        _cw[qp] = 0.;
        _Dht[qp] = 0.;
        break;
      }

      case 2: // Xi
      {
        // compute equivalent concrete age
        _eqv_age[qp] =
            _eqv_age_old[qp] + _dt / 86400.0 * 1.0 / (1.0 + std::pow((7.5 - 7.5 * H), 4.0));

        Real C = std::exp(855.0 / (T + 273.15));
        Real N_ct = 1.1;
        switch (_cement_type)
        {
          case 0: // cement_type = 1
            N_ct = 1.1;
            break;
          case 1: // cement_type = 2
            N_ct = 1.0;
            break;
          case 2: // cement_type = 3
            N_ct = 1.15;
            break;
          case 3: // cement_type = 4
            N_ct = 1.5;
            break;
          default: // cement_type = Unknown
            mooseError("Unknown cement type in mositure capacity calculations");
            break;
        }

        Real N_wc = 0.9;
        if (_water_to_cement < 0.3)
          N_wc = 0.99;
        else if (_water_to_cement >= 0.3 && _water_to_cement <= 0.7)
          N_wc = 0.33 + 2.2 * _water_to_cement;
        else
          N_wc = 1.87;

        Real N_te = 5.5;
        if (_eqv_age[qp] >= 5)
          N_te = 2.5 + 15.0 / _eqv_age[qp];

        Real n = N_te * N_wc * N_ct * 1.0;

        Real k = ((1.0 - 1.0 / n) * C - 1.0) / (C - 1.0);
        if (k < 0.0)
          k = 0.0;
        else if (k > 1.0)
          k = 1.0;

        Real V_ct = 0.9;
        switch (_cement_type)
        {
          case 0: // cement_type = 1
            V_ct = 0.9;
            break;
          case 1: // cement_type = 2
            V_ct = 1.0;
            break;
          case 2: // cement_type = 3
            V_ct = 0.85;
            break;
          case 3: // cement_type = 4
            V_ct = 0.6;
            break;
          default: // cement_type = Unknown
            mooseError("Unknown cement type in mositure capacity calculations");
            break;
        }

        Real V_wc = 0.985;
        if (_water_to_cement < 0.3)
          V_wc = 0.985;
        else if (_water_to_cement >= 0.3 && _water_to_cement <= 0.7)
          V_wc = 0.85 + 0.45 * _water_to_cement;
        else
          V_wc = 1.165;

        Real V_te = 0.024;
        if (_eqv_age[qp] >= 5)
          V_te = 0.068 - 0.22 / _eqv_age[qp];

        Real monolayer_factor = 1.0;
        Real Vm = V_te * V_wc * V_ct *
                  monolayer_factor; // here 1.0 is a factor is to account for the influence of
                                    // temperature in the calculation of monolayer capacity.

        Real W_cement = C * k * Vm * H / (1.0 - k * H) / (1.0 + (C - 1.0) * k * H);

        Real dWdH_cement = (C * k * Vm + W_cement * k * (1.0 + (C - 1.0) * k * H) -
                            W_cement * k * (1.0 - k * H) * (C - 1.0)) /
                           (1.0 - k * H) / (1.0 + (C - 1.0) * k * H);

        // compute mositure capacity dw/dH for aggregates
        Real n_agg = 1.5;
        switch (_aggregate_pore_type)
        {
          case 0: // aggregate_pore_type = dense
            n_agg = 1.5;
            break;
          case 1: // aggregate_pore_type = porous
            n_agg = 2.0;
            break;
          default:
            mooseError("Unknown aggregate pore structure");
            break;
        }

        n = 4.063 * n_agg;
        k = ((1.0 - 1.0 / n) * C - 1.0) / (C - 1.0);
        if (k < 0.0)
          k = 0.0;
        else if (k > 1.0)
          k = 1.0;

        Real V_agg = 0.05;
        switch (_aggregate_pore_type)
        {
          case 0: // aggregate_pore_type = dense
            V_agg = 0.05;
            break;
          case 1: // aggregate_pore_type = porous
            V_agg = 0.10;
            break;
          default:
            mooseError("Unknown aggregate pore structure");
            break;
        }

        Vm = 0.0647 * V_agg;

        Real W_agg = C * k * Vm * H / (1.0 - k * H) / (1.0 + (C - 1.0) * k * H);
        Real dWdH_agg = (C * k * Vm + W_agg * k * (1.0 + (C - 1.0) * k * H) -
                         W_agg * k * (1.0 - k * H) * (C - 1.0)) /
                        (1.0 - k * H) / (1.0 + (C - 1.0) * k * H);

        // compute weight percentages of agrregates and cement paste
        Real _f_agg = _aggregate_mass / (_aggregate_mass + _cement_mass);
        Real _f_cp = _cement_mass / (_aggregate_mass + _cement_mass);

        // compute combined dW/dH of concrete
        (*_moisture_capacity)[qp] = (_f_agg * dWdH_agg + _f_cp * dWdH_cement);

        // moisture diffusivity calculations
        Real gi = _agg_vol_fraction;
        Real wc = _water_to_cement;
        Real alfa_h = 1.05 - 3.8 * wc + 3.56 * std::pow(wc, 2.0);    // cm2/day
        Real betta_h = -14.4 + 50.4 * wc - 41.8 * std::pow(wc, 2.0); // cm2/day
        Real gamma_h = 31.3 - 136.0 * wc + 162.0 * std::pow(wc, 2.0);
        Real power1 = std::pow(10.0, gamma_h * (H - 1));
        Real power2 = std::pow(2.0, -1.0 * power1);

        Real Dhcp = alfa_h + betta_h * (1.0 - power2);  // cm2/day
        _Dh[qp] = Dhcp * (1 + gi / ((1 - gi) / 3 - 1)); // cm2/day
        _Dh[qp] = _Dh[qp] * 1e-4 / 86400;               // m2/s

        _ca[qp] = 0.;
        _cw[qp] = 0.;
        _Dht[qp] = 0.;
        break;
      }

      default:
      {
        mooseError("Unknown moisture diffusivity model");
        break;
      }
    }
  }
}
