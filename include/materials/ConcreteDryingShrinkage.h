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

#ifndef CONCRETEDRYINGSHRINKAGE_H
#define CONCRETEDRYINGSHRINKAGE_H

#include "ComputeEigenstrainBase.h"

class ConcreteDryingShrinkage;

template<>
InputParameters validParams<ConcreteDryingShrinkage>();

/** Computes drying shrinkage of concrete using a simple law */
class ConcreteDryingShrinkage : public ComputeEigenstrainBase
{
public:
  ConcreteDryingShrinkage(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties();
  virtual void computeQpEigenstrain();

  /// Relative humidity field (could be water saturation)
  const VariableValue & _humidity;

  /// value of the shrinkage when the humidity reach 0
  Real _shrinkage_coefficient;

  /// shape of the function used (must be > 0)
  Real _degree;

  bool _is_irreversible;
  /// value of the humidity below which shrinkage becomes irreversible
  Real _irreversibility_threshold;
  MaterialProperty<Real> * _irreversible_shrinkage;
  const MaterialProperty<Real> * _irreversible_shrinkage_old;
};

#endif // CONCRETEDRYINGSHRINKAGE_H
