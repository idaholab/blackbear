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

#include "NodalPostprocessor.h"

/**
 * Computes the total number density of clusters using the cluster dynamics
 * array variable used by ClusterDynamicsNodalKernel.
 */
class ClusterTotalDensity : public NodalPostprocessor
{
public:
  static InputParameters validParams();
  ClusterTotalDensity(const InputParameters & parameters);

  void initialize() override;
  void execute() override;
  void finalize() override;
  void threadJoin(const UserObject & y) override;
  Real getValue() const override;

protected:
  /// Cluster array variable
  const ArrayVariableValue & _clusters;

  /// Minimum cluster size to include in the total density
  const unsigned int _n_minimum;

  /// Atomic volume used to convert concentration sum to number density
  const Real _atomic_volume;

  Real _sum;
  Real _count;
};
