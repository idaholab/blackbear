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

#include "ClusterAverageRadius.h"

#include <cmath>

registerMooseObject("BlackBearApp", ClusterAverageRadius);

InputParameters
ClusterAverageRadius::validParams()
{
  InputParameters params = NodalPostprocessor::validParams();
  params.set<bool>("unique_node_execute") = true;
  params.addClassDescription("Computes the volume-weighted average cluster radius from the cluster "
                             "dynamics array variable.");
  params.addRequiredCoupledVar("clusters", "Cluster array concentration variable.");
  params.addParam<Real>("r1", 1.0, "Monomer radius scale where r_n = r1*n^(1/3)");
  params.addParam<unsigned int>(
      "n_minimum", 2, "Minimum cluster size to include in the average radius.");
  return params;
}

ClusterAverageRadius::ClusterAverageRadius(const InputParameters & parameters)
  : NodalPostprocessor(parameters),
    _clusters(coupledArrayValue("clusters")),
    _r1(getParam<Real>("r1")),
    _n_minimum(getParam<unsigned int>("n_minimum")),
    _radius_weighted_sum(0.0),
    _cluster_sum(0.0)
{
}

void
ClusterAverageRadius::initialize()
{
  _radius_weighted_sum = 0.0;
  _cluster_sum = 0.0;
}

void
ClusterAverageRadius::execute()
{
  // Array index i corresponds to cluster size n = i+1; start at i = n_minimum - 1
  auto min_cluster_index = _n_minimum - 1;
  for (auto i = min_cluster_index; i < _clusters[_qp].size(); ++i)
  {
    const unsigned int n = i + 1;
    const Real c_n = _clusters[_qp](i);
    _radius_weighted_sum += _r1 * std::cbrt(n) * c_n;
    _cluster_sum += c_n;
  }
}

void
ClusterAverageRadius::finalize()
{
  gatherSum(_radius_weighted_sum);
  gatherSum(_cluster_sum);
}

void
ClusterAverageRadius::threadJoin(const UserObject & y)
{
  const auto & rhs = static_cast<const ClusterAverageRadius &>(y);
  _radius_weighted_sum += rhs._radius_weighted_sum;
  _cluster_sum += rhs._cluster_sum;
}

Real
ClusterAverageRadius::getValue() const
{
  return _cluster_sum > 0.0 ? _radius_weighted_sum / _cluster_sum : 0.0;
}
