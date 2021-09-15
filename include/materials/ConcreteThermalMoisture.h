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

class ConcreteThermalMoisture : public Material
{
public:
  static InputParameters validParams();
  ConcreteThermalMoisture(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties();
  virtual void computeProperties();

private:
  MooseEnum _thermal_model;
  MooseEnum _moisture_model;

  // parameters for concrete mix proportions
  Real _cement_mass;
  Real _aggregate_mass;
  Real _water_to_cement;

  // parameters for constant thermal transport model
  Real _input_density;
  Real _input_specific_heat;
  Real _input_thermal_conductivity;

  // parameters for ASCE and KODUR thermal transport model
  MooseEnum _aggregate_type;

  // parameters for Bazant mositure transport model
  Real _D1;
  Real _n_power;
  Real _Hc;
  Real _alfa_Dht;

  // parameters for Mensi's moisture transport model
  Real _A;
  Real _B;

  // parameters for Xi's moisture transport model
  MooseEnum _cement_type;
  MooseEnum _aggregate_pore_type;
  Real _agg_vol_fraction;
  Real _cure_time;

  //@{ material properties asscociated with coupled mositure/thermal transfer through concrete
  MaterialProperty<Real> & _thermal_capacity;
  MaterialProperty<Real> & _thermal_conductivity;
  MaterialProperty<Real> & _ca;
  MaterialProperty<Real> & _cw;
  MaterialProperty<Real> * _moisture_capacity;
  ///@}

  MaterialProperty<Real> & _Dh;
  MaterialProperty<Real> & _Dht;
  MaterialProperty<Real> & _eqv_age;           // equvalent age used in Xi's mositure capacity model
  const MaterialProperty<Real> & _eqv_age_old; // equvalent age

  bool _has_rh; // coupled to relative humidity
  const VariableValue & _rh;

  bool _has_temperature;
  const VariableValue & _temp;
};
