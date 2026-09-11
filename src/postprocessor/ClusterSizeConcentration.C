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

#include "ClusterSizeConcentration.h"

registerMooseObject("BlackBearApp", ClusterSizeConcentration);

InputParameters
ClusterSizeConcentration::validParams()
{
  InputParameters params = NodalPostprocessor::validParams();
  params.set<bool>("unique_node_execute") = true;
  params.addClassDescription(
      "Computes the volume-averaged concentration of a single cluster size from the "
      "cluster dynamics array nodal variable.");
  params.addRequiredCoupledVar("clusters", "Cluster array concentration variable.");
  params.addParam<unsigned int>(
      "n_size",
      1,
      "Cluster size to compute volume averaged concentration; (default: 1, the monomer)");
  return params;
}

ClusterSizeConcentration::ClusterSizeConcentration(const InputParameters & parameters)
  : NodalPostprocessor(parameters),
    _clusters(coupledArrayValue("clusters")),
    _n_size(getParam<unsigned int>("n_size")),
    _sum(0.0),
    _count(0.0)
{
}

void
ClusterSizeConcentration::initialize()
{
  _sum = 0.0;
  _count = 0.0;
}

void
ClusterSizeConcentration::execute()
{
  _sum += _clusters[_qp](_n_size - 1);
  _count += 1.0;
}

void
ClusterSizeConcentration::finalize()
{
  gatherSum(_sum);
  gatherSum(_count);
}

void
ClusterSizeConcentration::threadJoin(const UserObject & y)
{
  const auto & rhs = static_cast<const ClusterSizeConcentration &>(y);
  _sum += rhs._sum;
  _count += rhs._count;
}

Real
ClusterSizeConcentration::getValue() const
{
  return _count > 0.0 ? _sum / _count : 0.0;
}
