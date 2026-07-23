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

#include "ClusterGroupingLayout.h"

#include "MooseError.h"

#include <algorithm>
#include <cmath>

ClusterGroupingLayout::ClusterGroupingLayout(unsigned int num_cluster_sizes,
                                             unsigned int explicit_max,
                                             bool use_grouping,
                                             ClusterGroupingSpacing spacing,
                                             unsigned int linear_width,
                                             Real geometric_ratio)
{
  initialize(num_cluster_sizes, explicit_max, use_grouping, spacing, linear_width, geometric_ratio);
}

void
ClusterGroupingLayout::initialize(unsigned int num_cluster_sizes,
                                  unsigned int explicit_max,
                                  bool use_grouping,
                                  ClusterGroupingSpacing spacing,
                                  unsigned int linear_width,
                                  Real geometric_ratio)
{
  if (num_cluster_sizes == 0)
    mooseError("ClusterGroupingLayout requires num_cluster_sizes > 0.");

  if (linear_width == 0)
    mooseError("ClusterGroupingLayout requires linear_width > 0.");

  if (spacing == ClusterGroupingSpacing::GEOMETRIC && geometric_ratio <= 1.0)
    mooseError("ClusterGroupingLayout with geometric spacing requires geometric_ratio > 1.");

  _num_cluster_sizes = num_cluster_sizes;
  _explicit_max =
      use_grouping ? std::max(1u, std::min(explicit_max, num_cluster_sizes)) : num_cluster_sizes;
  _component_count = 1;

  _explicit_component.assign(_num_cluster_sizes + 1, libMesh::invalid_uint);
  _group_of_size.assign(_num_cluster_sizes + 1, -1);
  _groups.clear();

  for (unsigned int n = 2; n <= _explicit_max; ++n)
    _explicit_component[n] = _component_count++;

  if (_explicit_max >= _num_cluster_sizes)
    return;

  unsigned int start = _explicit_max + 1;
  unsigned int current_width = linear_width;
  while (start <= _num_cluster_sizes)
  {
    ClusterGroupingBin bin;
    bin.start = start;
    bin.end = std::min(_num_cluster_sizes, start + current_width - 1);
    bin.width = static_cast<Real>(bin.end - bin.start + 1);
    bin.mean_x = integerMean(bin.start, bin.end);
    bin.sigma2 = integerSigma2(bin.start, bin.end);
    bin.l0_component = _component_count++;
    bin.l1_component = _component_count++;

    const int group_index = static_cast<int>(_groups.size());
    for (unsigned int n = bin.start; n <= bin.end; ++n)
      _group_of_size[n] = group_index;

    _groups.push_back(bin);
    start = bin.end + 1;

    if (spacing == ClusterGroupingSpacing::GEOMETRIC)
      current_width = std::max(
          current_width + 1, static_cast<unsigned int>(std::ceil(current_width * geometric_ratio)));
  }
}

bool
ClusterGroupingLayout::containsSize(unsigned int n) const
{
  return n >= 1 && n <= _num_cluster_sizes;
}

bool
ClusterGroupingLayout::isExplicitSize(unsigned int n) const
{
  return containsSize(n) && (n == 1 || n <= _explicit_max);
}

bool
ClusterGroupingLayout::isGroupedSize(unsigned int n) const
{
  return containsSize(n) && n > _explicit_max;
}

unsigned int
ClusterGroupingLayout::explicitComponent(unsigned int n) const
{
  if (!isExplicitSize(n))
    mooseError("Requested explicit component for non-explicit cluster size ", n, ".");
  return n == 1 ? 0 : _explicit_component[n];
}

int
ClusterGroupingLayout::groupIndexForSize(unsigned int n) const
{
  if (!containsSize(n))
    mooseError("Requested group index for out-of-range cluster size ", n, ".");
  return _group_of_size[n];
}

const ClusterGroupingBin &
ClusterGroupingLayout::group(unsigned int group_index) const
{
  if (group_index >= _groups.size())
    mooseError(
        "Requested group index ", group_index, " but only ", _groups.size(), " groups exist.");
  return _groups[group_index];
}

const ClusterGroupingBin &
ClusterGroupingLayout::groupForSize(unsigned int n) const
{
  const int group_index = groupIndexForSize(n);
  if (group_index < 0)
    mooseError("Requested grouped bin for explicit cluster size ", n, ".");
  return _groups[group_index];
}

Real
ClusterGroupingLayout::integerMean(unsigned int start, unsigned int end)
{
  return 0.5 * (static_cast<Real>(start) + static_cast<Real>(end));
}

Real
ClusterGroupingLayout::integerSigma2(unsigned int start, unsigned int end)
{
  const Real width = static_cast<Real>(end - start + 1);
  if (width <= 1.0)
    return 0.0;

  Real sum1 = 0.0;
  Real sum2 = 0.0;
  for (unsigned int n = start; n <= end; ++n)
  {
    const Real x = static_cast<Real>(n);
    sum1 += x;
    sum2 += x * x;
  }

  const Real mean = sum1 / width;
  return sum2 / width - mean * mean;
}
