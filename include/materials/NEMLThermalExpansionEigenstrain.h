#ifndef COMPUTETHERMALEXPANSIONEIGENSTRAINNEML_H
#define COMPUTETHERMALEXPANSIONEIGENSTRAINNEML_H

#include "ComputeThermalExpansionEigenstrainBase.h"

#include "neml_interface.h"

class ComputeThermalExpansionEigenstrainNEML;

template <>
InputParameters validParams<ComputeThermalExpansionEigenstrainNEML>();

/**
 *  ComputeThermalExpansionEigenstrainNEML computes the thermal expansion
 *  strain from the instantaneous CTE provided by a NEML model
 */
class ComputeThermalExpansionEigenstrainNEML: public ComputeThermalExpansionEigenstrainBase
{
 public:
  ComputeThermalExpansionEigenstrainNEML(const InputParameters & parameters);
  virtual void initQpStatefulProperties() override;

 protected:
  virtual void computeThermalStrain(Real & thermal_strain, Real & instantaneous_cte) override;

 protected:
  FileName _fname;
  std::string _mname;
  std::unique_ptr<neml::NEMLModel> _model;

  MaterialProperty<Real> & _tstrain;
  const MaterialProperty<Real> & _tstrain_old;

  const VariableValue & _temperature_old;
};












#endif // COMPUTETHERMALEXPANSIONEIGENSTRAINNEML_H
