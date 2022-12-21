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

#include "MultiParameterPlasticityStressUpdate.h"

class DamagePlasticityStressUpdate : public MultiParameterPlasticityStressUpdate
{
public:
  static InputParameters validParams();
  DamagePlasticityStressUpdate(const InputParameters & parameters);

  /**
   * Does the model require the elasticity tensor to be isotropic?
   */
  bool requiresIsotropicTensor() override { return true; }

protected:
  virtual void initQpStatefulProperties() override;
  virtual void finalizeReturnProcess(const RankTwoTensor & rotation_increment) override;

private:
  const Real _f_tol;
  const Real _alfa;
  const Real _alfa_p;
  const Real _s0;

  const Real _Chi;
  const Real _Dt;
  const Real _ft;
  const Real _FEt;

  const Real _fyc;
  const Real _Dc;
  const Real _fc;
  const Real _FEc;

  const Real _at;
  const Real _ac;
  const Real _zt;
  const Real _zc;
  const Real _dPhit;
  const Real _dPhic;
  const Real _sqrtPhit_max;
  const Real _sqrtPhic_max;
  const Real _dt_bt;
  const Real _dc_bc;
  const Real _ft0;
  const Real _fc0;
  const Real _small_smoother2;

  const Real _sqrt3;

  /// Whether to provide an estimate of the returned stress, based on perfect plasticity
  const bool _perfect_guess;

  /// Eigenvectors of the trial stress as a RankTwoTensor, in order to rotate the returned stress back to stress space
  RankTwoTensor _eigvecs;


  MaterialProperty<Real> & _intnl0;
  MaterialProperty<Real> & _intnl1;
  MaterialProperty<Real> & _ele_len;
  MaterialProperty<Real> & _gt;
  MaterialProperty<Real> & _gc;

  MaterialProperty<Real> & _tD;
  MaterialProperty<Real> & _cD;
  MaterialProperty<Real> & _D;
  MaterialProperty<Real> & _min_ep;
  MaterialProperty<Real> & _mid_ep;
  MaterialProperty<Real> & _max_ep;
  MaterialProperty<Real> & _sigma0;
  MaterialProperty<Real> & _sigma1;
  MaterialProperty<Real> & _sigma2;

  Real ft(const std::vector<Real> & intnl) const;  /// tensile strength
  Real dft(const std::vector<Real> & intnl) const; /// d(tensile strength)/d(intnl)
  Real fc(const std::vector<Real> & intnl) const;  /// compressive strength
  Real dfc(const std::vector<Real> & intnl) const; /// d(compressive strength)/d(intnl)
  Real beta(const std::vector<Real> & intnl) const;
  Real dbeta0(const std::vector<Real> & intnl) const;
  Real dbeta1(const std::vector<Real> & intnl) const;
  void weighfac(const std::vector<Real> & stress_params, Real & wf) const; /// weight factor
  void dweighfac(const std::vector<Real> & stress_params,
                 Real & wf,
                 std::vector<Real> & dwf) const; /// d(weight factor)/d(stress)
  Real damageVar(const std::vector<Real> & stress_params, const std::vector<Real> & intnl) const;

  void computeStressParams(const RankTwoTensor & stress,
                           std::vector<Real> & stress_params) const override;

  std::vector<RankTwoTensor> dstress_param_dstress(const RankTwoTensor & stress) const override;

  std::vector<RankFourTensor> d2stress_param_dstress(const RankTwoTensor & stress) const override;

  void setEffectiveElasticity(const RankFourTensor & Eijkl) override;

  virtual void preReturnMapV(const std::vector<Real> & trial_stress_params,
                             const RankTwoTensor & stress_trial,
                             const std::vector<Real> & intnl_old,
                             const std::vector<Real> & yf,
                             const RankFourTensor & Eijkl) override;

  virtual void setStressAfterReturnV(const RankTwoTensor & stress_trial,
                                     const std::vector<Real> & stress_params,
                                     Real gaE,
                                     const std::vector<Real> & intnl,
                                     const yieldAndFlow & smoothed_q,
                                     const RankFourTensor & Eijkl,
                                     RankTwoTensor & stress) const override;

  void yieldFunctionValuesV(const std::vector<Real> & stress_params,
                            const std::vector<Real> & intnl,
                            std::vector<Real> & yf) const override;

  void computeAllQV(const std::vector<Real> & stress_params,
                    const std::vector<Real> & intnl,
                    std::vector<yieldAndFlow> & all_q) const override;

  virtual void flowPotential(const std::vector<Real> & stress_params,
                             const std::vector<Real> & intnl,
                             std::vector<Real> & r) const;

  virtual void dflowPotential_dstress(const std::vector<Real> & stress_params,
                                      const std::vector<Real> & intnl,
                                      std::vector<std::vector<Real>> & dr_dstress) const;

  virtual void dflowPotential_dintnl(const std::vector<Real> & stress_params,
                                     const std::vector<Real> & intnl,
                                     std::vector<std::vector<Real>> & dr_dintnl) const;

  virtual void hardPotential(const std::vector<Real> & stress_params,
                             const std::vector<Real> & intnl,
                             std::vector<Real> & h) const;

  virtual void dhardPotential_dstress(const std::vector<Real> & stress_params,
                                      const std::vector<Real> & intnl,
                                      std::vector<std::vector<Real>> & dh_dsig) const;

  virtual void dhardPotential_dintnl(const std::vector<Real> & stress_params,
                                     const std::vector<Real> & intnl,
                                     std::vector<std::vector<Real>> & dh_dintnl) const;

  void initialiseVarsV(const std::vector<Real> & trial_stress_params,
                       const std::vector<Real> & intnl_old,
                       std::vector<Real> & stress_params,
                       Real & gaE,
                       std::vector<Real> & intnl) const;

  void setIntnlValuesV(const std::vector<Real> & trial_stress_params,
                       const std::vector<Real> & current_stress_params,
                       const std::vector<Real> & intnl_old,
                       std::vector<Real> & intnl) const override;

  void setIntnlDerivativesV(const std::vector<Real> & trial_stress_params,
                            const std::vector<Real> & current_stress_params,
                            const std::vector<Real> & intnl,
                            std::vector<std::vector<Real>> & dintnl) const override;

  virtual void consistentTangentOperatorV(const RankTwoTensor & stress_trial,
                                          const std::vector<Real> & trial_stress_params,
                                          const RankTwoTensor & stress,
                                          const std::vector<Real> & stress_params,
                                          Real gaE,
                                          const yieldAndFlow & smoothed_q,
                                          const RankFourTensor & Eijkl,
                                          bool compute_full_tangent_operator,
                                          const std::vector<std::vector<Real>> & dvar_dtrial,
                                          RankFourTensor & cto) override;
};
