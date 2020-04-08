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

#include "ComputeThermalExpansionEigenstrainBase.h"

#include "neml_interface.h"



/**
 *  NEMLThermalExpansionEigenstrain computes the thermal expansion
 *  strain from the instantaneous CTE provided by a NEML model
 */
class NEMLThermalExpansionEigenstrain : public ComputeThermalExpansionEigenstrainBase
{
public:
  static InputParameters validParams();
  NEMLThermalExpansionEigenstrain(const InputParameters & parameters);
  virtual void initQpStatefulProperties() override;

protected:
  virtual void computeThermalStrain(Real & thermal_strain, Real & instantaneous_cte) override;

protected:
  /// File name of the NEML XML database
  FileName _fname;
  /// Model name in NEML XML database
  std::string _mname;
  /// NEML model
  std::unique_ptr<neml::NEMLModel> _model;

  /// Thermal strain
  ///@{
  MaterialProperty<Real> & _tstrain;
  const MaterialProperty<Real> & _tstrain_old;
  ///@}

  /// Old state of the coupled temperature
  const VariableValue & _temperature_old;
};
