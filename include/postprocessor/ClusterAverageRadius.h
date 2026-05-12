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
 * Computes the volume-weighted average cluster radius using the cluster
 * dynamics array variable used by ClusterDynamicsNodalKernel.
 * Only cluster sizes n >= n_minimum are included in the average.
 *
 * Average radius: sum_{n >= n_min} r_n * C_n / sum_{n >= n_min} C_n
 * where r_n = r1 * n^(1/3).
 */
class ClusterAverageRadius : public NodalPostprocessor
{
public:
  static InputParameters validParams();
  ClusterAverageRadius(const InputParameters & parameters);

  void initialize() override;
  void execute() override;
  void finalize() override;
  void threadJoin(const UserObject & y) override;
  Real getValue() const override;

protected:
  /// Cluster array variable
  const ArrayVariableValue & _clusters;

  /// Monomer radius scale: r_n = r1 * n^(1/3)
  const Real _r1;

  /// Minimum cluster size to include in the average
  const unsigned int _n_minimum;

  Real _radius_weighted_sum;
  Real _cluster_sum;
};
