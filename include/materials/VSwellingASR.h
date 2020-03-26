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

#pragma once

#include "VolumetricModel.h"
#include "SymmTensor.h"
//#include <vector>

class VSwellingASR : public VolumetricModel
{
public:
  VSwellingASR(const InputParameters & parameters);

  void initStatefulProperties(unsigned n_points);

  virtual void modifyStrain(const unsigned int qp,
                            const Real sclae_factor,
                            SymmTensor & strain_increment,
                            SymmTensor & dstrain_increment_dT);

  virtual bool applyASRStrain(unsigned qp, const Real v0OverVOld, SymmTensor & strain_increment);
  virtual Real computeResidual(unsigned qp, Real scalar);
  virtual Real computeDerivative(unsigned qp, Real scalar);

  // Redistribution ASR anisotropic weights code
  Real Weight(Real sig_l, Real sig_m, Real sig_k, Real sig_u, Real f_c, Real f_t);

  // Calculate N1, N2, N3, N4, N5, N6
  int Find_neighbors(Real sig, Real f_c, Real sig_u, Real f_t);

  // Calculate a an b
  Real Cal_ab(Real ab1, Real ab2, Real ab3, int pbound);

  // Calcuate Sig
  Real Cal_sig(Real sig, Real sig_u, int pbound, Real f_c, Real f_t);

  // Calculate W
  Real Cal_W(int N1,
             int N2,
             int N3,
             int N4,
             int N5,
             int N6,
             Real a,
             Real b,
             Real sig_l,
             Real sig_m,
             Real sig_k,
             Real f_t,
             Real sig_u,
             Real f_c);

  // Calculate W_i
  Real W_i(int N1,
           int N5,
           int N6,
           Real sig_k,
           std::vector<std::vector<Real>> & Table,
           Real f_t,
           Real sig_u,
           Real f_c);

private:
  const bool _has_temp; // coupled to temperature
  const VariableValue & _temperature;
  const VariableValue & _temperature_old;

  bool _has_rh; // coupled to relative humidity
  const VariableValue & _rh;
  Real _m_power;

  // parameters associated with ASR volumtric explasion
  MooseEnum _ASR_formulation;

  Real _ASR_final_vstrain;
  Real _tau_c_T0;
  Real _tau_L_T0;
  Real _Uc;
  Real _UL;
  Real _ASR_T0;

  Real _f_compress;
  Real _alpha;
  Real _f_tensile;
  Real _f_u;

  // parameters for ASR gel adsorption due to tensile cracking
  Real _gamma_tensile;
  Real _gamma_residual;

  // parameters for ASR gel adsorption due to compressive cracking
  Real _beta;

  // ASR dependent tensile strength
  bool _ASR_dependent_tensile_strength;
  Real _beta_f_tensile;

  // parameters of sub-Newton iterations
  const unsigned int _max_its;
  const bool _output_iteration_info;
  const bool _output_iteration_info_on_error;
  const Real _relative_tolerance;
  const Real _absolute_tolerance;

  MaterialProperty<Real> & _ASR_extent;
  const MaterialProperty<Real> & _ASR_extent_old;

  MaterialProperty<Real> & _ASR_volumetric_strain;
  const MaterialProperty<Real> & _ASR_volumetric_strain_old;

  MaterialProperty<SymmTensor> & _ASR_strain;
  const MaterialProperty<SymmTensor> & _ASR_strain_old;

  //  const MaterialProperty<SymmTensor> & _stress_old_prop;
  const MaterialProperty<SymmTensor> & _stress_prop;
};

template <>
InputParameters validParams<VSwellingASR>();
