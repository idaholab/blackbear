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

#include "SecondaryAqueousSpeciesTimeIntegration.h"
#include "Material.h"

/**
 * This function defines the valid parameters for
 * this Kernel and their default values
 */
template<>
InputParameters validParams<SecondaryAqueousSpeciesTimeIntegration>()
{
  InputParameters params = validParams<Kernel>();
  params.addParam<Real>("weight",1.0,"The weight of the equilibrium species");
  params.addParam<Real>("log_k",0.0,"The equilibrium constaant of this equilibrium species in dissociateion reaction");

  params.addParam<Real>("sto_u",1.0,"The stochiomentic coef of the primary variable this kernel operats on");

  params.addParam<std::vector<Real> >("sto_v","The stochiometric coefficients of coupled primary species");
  params.addCoupledVar("v", "Coupled primary species constituting the equalibrium species");

  return params;
}


SecondaryAqueousSpeciesTimeIntegration::SecondaryAqueousSpeciesTimeIntegration(const std::string & name, InputParameters parameters) :
    Kernel(name,parameters),
    _weight(getParam<Real>("weight")),
    _log_k(getParam<Real>("log_k")),
    _sto_u(getParam<Real>("sto_u")),
    _sto_v(getParam<std::vector<Real> >("sto_v")),
    _porosity(getMaterialProperty<Real>("porosity")),
    _u_old(valueOld())
{
  int n = coupledComponents("v");
  _vars.resize(n);
  _v_vals.resize(n);
  _v_vals_old.resize(n);

  for (unsigned int i=0; i < _v_vals.size(); ++i)
  {
    _vars[i] = coupled("v", i);
    _v_vals[i] = &coupledValue("v", i);
    _v_vals_old[i] = & coupledValueOld("v", i);
  }
}

Real SecondaryAqueousSpeciesTimeIntegration::computeQpResidual()
{
  Real _val_new = std::pow(10.0,_log_k)*std::pow(_u[_qp],_sto_u);
  Real _val_old = std::pow(10.0,_log_k)*std::pow(_u_old[_qp],_sto_u);

  if (_v_vals.size())
  {
    for (unsigned int i=0; i<_v_vals.size(); ++i)
    {
      _val_new *= std::pow((*_v_vals[i])[_qp],_sto_v[i]);
      _val_old *= std::pow((*_v_vals_old[i])[_qp],_sto_v[i]);
    }
  }

  return _porosity[_qp]*_weight*_test[_i][_qp]*(_val_new-_val_old)/_dt;
}

Real SecondaryAqueousSpeciesTimeIntegration::computeQpJacobian()
{
  Real _val_new = std::pow(10.0,_log_k)*_sto_u*std::pow(_u[_qp],_sto_u-1.0)*_phi[_j][_qp];

  if (_v_vals.size())
    for (unsigned int i=0; i<_v_vals.size(); ++i)
      _val_new *= std::pow((*_v_vals[i])[_qp],_sto_v[i]);

  return _porosity[_qp]*_test[_i][_qp]*_weight*_val_new/_dt;
}


Real SecondaryAqueousSpeciesTimeIntegration::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real _val_new = std::pow(10.0,_log_k) * std::pow(_u[_qp],_sto_u);

  if (_vars.size())
  {
    for (unsigned int i=0; i<_vars.size(); ++i)
    {
      if (jvar == _vars[i])
        _val_new *= _sto_v[i]*std::pow((*_v_vals[i])[_qp],_sto_v[i]-1.0)*_phi[_j][_qp];
      else
        _val_new *= std::pow((*_v_vals[i])[_qp],_sto_v[i]);
    }

    return _porosity[_qp]*_test[_i][_qp]*_weight*_val_new/_dt;
  }
  else
    return 0.0;
}
