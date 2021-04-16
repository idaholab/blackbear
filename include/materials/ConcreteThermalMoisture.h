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

#include "Material.h"

#pragma once

class PorousMediaBase : public Material
{
public:
  static InputParameters validParams();
  PorousMediaBase(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties();
  virtual void computeProperties();

  MooseEnum _thermal_conductivity_model;
  MooseEnum _thermal_capacity_model;
  MooseEnum _aggregate_type;
  MooseEnum _cement_type;
  MooseEnum _aggregate_pore_type;

  MooseEnum _moisture_diffusivity_model;

private:
  Real _input_initial_diffusivity;
  Real _input_initial_porosity;
  Real _input_initial_permeability;
  Real _input_initial_storativity;
  Real _input_bulk_density;

  ///@{ minerals in matrix with potential dissolution/precipitation reactions
  std::vector<Real> _mineral_molecular_weight;
  std::vector<Real> _mineral_density;
  std::vector<Real> _initial_mineral_conc;
  ///@}

  Real _water_to_cement;
  Real _cure_time;
  Real _cement_mass;
  Real _aggregate_mass;

  // parameters for Bazant mositure diffusivity model
  Real _D1;
  Real _n_power;

  // parameters for Mensi's moisture diffusivity model
  Real _A;
  Real _B;
  Real _C0;

  // parameters for Xi's moisture diffusivity model
  Real _agg_vol_fraction;

  Real _input_density_of_concrete;
  Real _input_specific_heat_of_concrete;
  Real _input_thermal_conductivity_of_concrete;

  Real _alfa_Dht;

  ///@{ some intrinsic material properties
  MaterialProperty<Real> & _diffusivity;
  MaterialProperty<Real> & _permeability;
  MaterialProperty<Real> & _storativity;
  MaterialProperty<Real> & _porosity;
  ///@}

  //@{ material properties asscociated with coupled mositure/thermal transfer through concrete
  MaterialProperty<Real> & _thermal_capacity;
  MaterialProperty<Real> & _thermal_conductivity;
  MaterialProperty<Real> & _ca;
  MaterialProperty<Real> & _moisture_capacity;
  MaterialProperty<Real> & _moisture_content;
  MaterialProperty<Real> & _cw;
  MaterialProperty<RealGradient> & _darcy_moisture_flux;
  ///@}

  MaterialProperty<Real> & _Dh;
  MaterialProperty<Real> & _Dht;
  MaterialProperty<Real> & _WH; // hydrated water mass per m^3 of concrete
  MaterialProperty<Real> & _te; // equvalent hydration time -used in compute dehydrated water
  const MaterialProperty<Real> & _te_old;      // equvalent hydration time
  MaterialProperty<Real> & _eqv_age;           // equvalent age used in Xi's mositure capacity model
  const MaterialProperty<Real> & _eqv_age_old; // equvalent age

  bool _has_rh; // coupled to relative humidity
  const VariableValue & _rh;
  const VariableGradient & _grad_rh;

  bool _has_temperature;
  const VariableValue & _temp;

  /// coupled to mineral concentrations(i.e., amount in porous matrix)
  std::vector<const VariableValue *> _vals;
};
