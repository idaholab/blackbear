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
 * Computes the volume-averaged concentration of a single cluster size from
 * the cluster dynamics array nodal variable used by ClusterDynamicsNodalKernel.
 */
class ClusterSizeConcentration : public NodalPostprocessor
{
public:
  static InputParameters validParams();
  ClusterSizeConcentration(const InputParameters & parameters);

  void initialize() override;
  void execute() override;
  void finalize() override;
  void threadJoin(const UserObject & y) override;
  Real getValue() const override;

protected:
  /// Cluster array variable
  const ArrayVariableValue & _clusters;

  /// Cluster size to compute average for.
  const unsigned int _n_size;

  Real _sum;
  Real _count;
};
