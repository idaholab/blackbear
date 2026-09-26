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

#include "ClusterGroupingLayout.h"
#include "NodalPostprocessor.h"

class ClusterTotalDensityGrouped : public NodalPostprocessor
{
public:
  static InputParameters validParams();
  ClusterTotalDensityGrouped(const InputParameters & parameters);

  void initialize() override;
  void execute() override;
  void finalize() override;
  void threadJoin(const UserObject & y) override;
  Real getValue() const override;

protected:
  Real concentrationAt(unsigned int n) const;
  bool groupedBinNonnegative(const ClusterGroupingBin & bin) const;

  const ArrayVariableValue & _clusters;
  const unsigned int _n_minimum;
  const Real _atomic_volume;
  const ClusterGroupingLayout _layout;
  const bool _enforce_group_nonnegative;
  const Real _group_nonnegative_tolerance_factor;

  Real _sum;
  Real _count;
};
