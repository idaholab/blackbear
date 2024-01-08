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

  bool requiresIsotropicTensor() override { return true; }

protected:
  virtual void initQpStatefulProperties() override;
  virtual void finalizeReturnProcess(const RankTwoTensor & rotation_increment) override;

private:
  ///Yield function tolerance (user parameter)
  const Real _f_tol;
  ///dimensionless parameter involving biaxial & uniaxial compressive strengths (user parameter)
  const Real _alfa;
  ///dilatancy factor (user parameter)
  const Real _alfa_p;
  ///stiffness recovery factor (user parameter)
  const Real _s0;
  ///ft_ep_slope_factor_at_zero_ep (user parameter)
  const Real _dstress_dep;
  ///tensile damage at half tensile strength (user parameter)
  const Real _Dt;
  ///yield strength in tension (user parameter)
  const Real _ft;
  ///fracture_energy in tension (user parameter)
  const Real _FEt;
  ///Area under the stress and plastic strain in  uniaxial tension (user parameter)
  const Real _area_S_ep;
  ///yield strength in compression (user parameter)
  const Real _fyc;
  ///compressive damage at maximum compressive strength (user parameter)
  const Real _Dc;
  ///maximum strength in compression (user parameter)
  const Real _fc;
  ///fracture energy in compression (user parameter)
  const Real _FEc;
  /// Maximum stress in tension without damage
  const Real _ft0;
  /// Maximum stress in compression without damage
  const Real _fc0;

  ///@{
  /** The following variables are intermediate and are calculated based on the user parameters given
   * above */
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

  ///@}

  /// Intermediate variable calculated using  user parameter tip_smoother
  const Real _small_smoother2;

  /// Square root of 3
  const Real _sqrt3;

  /// Whether to provide an estimate of the returned stress, based on perfect plasticity
  const bool _perfect_guess;

  /// Eigenvectors of the trial stress as a RankTwoTensor, in order to rotate the returned stress back to stress space
  RankTwoTensor _eigvecs;
  ///damage state in tension
  MaterialProperty<Real> & _intnl0;
  ///damage state in compression
  MaterialProperty<Real> & _intnl1;
  ///element length
  MaterialProperty<Real> & _ele_len;
  ///fracture energy in tension
  MaterialProperty<Real> & _gt;
  ///fracture energy in compression
  MaterialProperty<Real> & _gc;
  ///tensile damage
  MaterialProperty<Real> & _tD;
  ///compression damage
  MaterialProperty<Real> & _cD;
  ///damage variable
  MaterialProperty<Real> & _D;
  ///minimum Eigen value of plastic strain
  MaterialProperty<Real> & _min_ep;
  ///middle Eigen value of plastic strain
  MaterialProperty<Real> & _mid_ep;
  ///maximum Eigen value of plastic strain
  MaterialProperty<Real> & _max_ep;
  ///damaged minimum principal stress
  MaterialProperty<Real> & _sigma0;
  ///damaged mid value of principal stress
  MaterialProperty<Real> & _sigma1;
  ///damaged maximum principal stress
  MaterialProperty<Real> & _sigma2;
  /**
   * Obtain the undamaged strength
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return value of ft (tensile strength)
   */
  Real fbar(const Real & f0,
            const Real & a,
            const Real & exponent,
            const Real & kappa) const;
  // Real ftbar(const std::vector<Real> & intnl) const;
  /**
   * Obtain the partial derivative of the undamaged tensile strength to the damage state
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return value of dft (partial derivative of the tensile strength to the damage state)
   */
  Real dfbar_dkappa(const Real & f0,
                    const Real & a,
                    const Real & exponent,
                    const Real & kappa) const;

  // /**
  //  * Obtain the partial derivative of the undamaged tensile strength to the damage state
  //  * @param intnl (Array containing damage states in tension and compression, respectively)
  //  * @return value of dft (partial derivative of the tensile strength to the damage state)
  //  */
  // Real dftbar(const std::vector<Real> & intnl) const;

  // /**
  //  * Obtain the undamaged conpressive strength
  //  * @param intnl (Array containing damage states in tension and compression, respectively)
  //  * @return value of fc (compressive strength)
  //  */
  // Real fcbar(const std::vector<Real> & intnl) const;

  // /**
  //  * Obtain the partial derivative of the undamaged compressive strength to the damage state
  //  * @param intnl (Array containing damage states in tension and compression, respectively)
  //  * @return value of dfc
  //  */
  // Real dfcbar(const std::vector<Real> & intnl) const;
  // /**
  //  * Obtain the damaged tensile strength
  //  * @param intnl (Array containing damage states in tension and compression, respectively)
  //  * @return value of ft (tensile strength)
  //  */
  Real f(const Real & f0,
         const Real & a,
         const Real & kappa) const;

  // /**
  //  * Obtain the partial derivative of the damaged tensile strength to the damage state
  //  * @param intnl (Array containing damage states in tension and compression, respectively)
  //  * @return value of dft (partial derivative of the tensile strength to the damage state)
  //  */
  // Real dft(const std::vector<Real> & intnl) const;

  // /**
  //  * Obtain the damaged compressive strength
  //  * @param intnl (Array containing damage states in tension and compression, respectively)
  //  * @return value of fc (compressive strength)
  //  */
  // Real fc(const std::vector<Real> & intnl) const;

  // /**
  //  * Obtain the partial derivative of the damaged compressive strength to the damage state
  //  * @param intnl (Array containing damage states in tension and compression, respectively)
  //  * @return value of dfc
  //  */
  // Real dfc(const std::vector<Real> & intnl) const;

  /**
   * Obtain the partial derivative of the undamaged  strength to the damage state
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return value of dft (partial derivative of the tensile strength to the damage state)
   */
  Real df_dkappa(const Real & f0,
                 const Real & a,
                 const Real & kappa) const;
  /**
   * beta is a dimensionless constant, which is a component of the yield function
   * It is defined in terms of tensile strength, compressive strength, and another
   * dimensionless constant alpha (See Eqn. 37 in Lee (1998))
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return value of beta
   */
  Real beta(const std::vector<Real> & intnl) const;

    /**
   * dbeta_dkappa is a derivative of beta wrt. kappa (plastic damage variable)
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return value of dbeta_dkappa
   */
  Real dbeta_dkappa(const std::vector<Real> & intnl) const;

  /**
   * dbeta0 is a derivative of beta wrt. tensile strength (ft)
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return value of dbeta0
   */
  Real dbeta0(const std::vector<Real> & intnl) const;

  /**
   * dbeta1 is a derivative of beta wrt. compressive strength (fc)
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return value of dbeta1
   */
  Real dbeta1(const std::vector<Real> & intnl) const;

  /**
   * weighfac is the weight factor, defined interms of the ratio of sum of  principal stresses
   * to the sum of absolute value of the principal stresses
   * @param stress_params is the array of principal stresses
   * @return wf
   */
  void weighfac(const std::vector<Real> & stress_params, Real & wf) const;

  /**
   * dweighfac is the derivative of the weight factor
   * @param stress_params is the array of principal stresses
   * @param wf is the weight factor
   * @return dwf
   */
  void dweighfac(const std::vector<Real> & stress_params, Real & wf, std::vector<Real> & dwf) const;

  /**
   * damageVar is the  degradation damage variable as defined in Eqn. 43 in Lee (1998)
   * @param stress_params is the array of principal stresses
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return value of damageVar
   */
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

  /**
   * This function calculates the flow potential
   * @param stress_params is the array of principal stresses
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return r (flowpotential)
   */
  virtual void flowPotential(const std::vector<Real> & stress_params,
                             const std::vector<Real> & intnl,
                             std::vector<Real> & r) const;

  /**
   * This function calculates the derivative of the flow potential with the stress
   * @param stress_params is the array of principal stresses
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return dr_dstress (dflowpotential_dstress)
   */
  virtual void dflowPotential_dstress(const std::vector<Real> & stress_params,
                                      const std::vector<Real> & intnl,
                                      std::vector<std::vector<Real>> & dr_dstress) const;
  /**
   * This function calculates the derivative of the flow potential with the damage states
   * @param stress_params is the array of principal stresses
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return dr_dintnl (dflowPotential_dintnl)
   */
  virtual void dflowPotential_dintnl(const std::vector<Real> & stress_params,
                                     const std::vector<Real> & intnl,
                                     std::vector<std::vector<Real>> & dr_dintnl) const;
  /**
   * This function calculates the hardening potential
   * @param stress_params is the array of principal stresses
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return h (hardening Potential)
   */
  virtual void hardPotential(const std::vector<Real> & stress_params,
                             const std::vector<Real> & intnl,
                             std::vector<Real> & h) const;
  /**
   * This function calculates the derivative of the hardening potential with the stress
   * @param stress_params is the array of principal stresses
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return dh_dsig (dhardPotential_dstress)
   */
  virtual void dhardPotential_dstress(const std::vector<Real> & stress_params,
                                      const std::vector<Real> & intnl,
                                      std::vector<std::vector<Real>> & dh_dsig) const;
  /**
   * This function calculates the derivative of the hardening potential with the damage states
   * @param stress_params is the array of principal stresses
   * @param intnl (Array containing damage states in tension and compression, respectively)
   * @return dh_dintnl (dhardPotential_dintnl)
   */
  virtual void dhardPotential_dintnl(const std::vector<Real> & stress_params,
                                     const std::vector<Real> & intnl,
                                     std::vector<std::vector<Real>> & dh_dintnl) const;
  /**
   * This function updates the damage states
   * @param stress_params is the array of principal stresses
   * @param trial_stress_params is the trial values of the principal stresses
   * @param intnl_old (Array containing previous step's damage states in tension and compression,
   * respectively)
   * @return no particular return values, updates internal variables
   */
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
