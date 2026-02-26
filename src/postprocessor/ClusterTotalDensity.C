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

#include "ClusterTotalDensity.h"

registerMooseObject("BlackBearApp", ClusterTotalDensity);

InputParameters
ClusterTotalDensity::validParams()
{
  InputParameters params = NodalPostprocessor::validParams();
  params.set<bool>("unique_node_execute") = true;
  params.addClassDescription(
      "Computes the total number density of clusters from the cluster dynamics "
      "array variable. Only cluster sizes n >= n_minimum are summed, divided by atomic_volume.");
  params.addRequiredCoupledVar("clusters", "Cluster array concentration variable.");
  params.addParam<unsigned int>(
      "n_minimum",
      1,
      "Minimum cluster size to include in the total density (default: 1, includes monomers)");
  params.addParam<Real>(
      "atomic_volume", 1.0, "Atomic volume used to convert concentration sum to number density");
  return params;
}

ClusterTotalDensity::ClusterTotalDensity(const InputParameters & parameters)
  : NodalPostprocessor(parameters),
    _clusters(coupledArrayValue("clusters")),
    _n_minimum(getParam<unsigned int>("n_minimum")),
    _atomic_volume(getParam<Real>("atomic_volume")),
    _sum(0.0),
    _count(0.0)
{
}

void
ClusterTotalDensity::initialize()
{
  _sum = 0.0;
  _count = 0.0;
}

void
ClusterTotalDensity::execute()
{
  Real node_total = 0.0;

  // Array index i corresponds to cluster size n = i+1; start at i = n_minimum - 1
  for (auto i = _n_minimum - 1; i < _clusters[_qp].size(); ++i)
    node_total += _clusters[_qp](i);

  _sum += node_total;
  _count += 1.0;
}

void
ClusterTotalDensity::finalize()
{
  gatherSum(_sum);
  gatherSum(_count);
}

void
ClusterTotalDensity::threadJoin(const UserObject & y)
{
  const auto & rhs = static_cast<const ClusterTotalDensity &>(y);
  _sum += rhs._sum;
  _count += rhs._count;
}

Real
ClusterTotalDensity::getValue() const
{
  return _count > 0.0 ? _sum / (_count * _atomic_volume) : 0.0;
}
