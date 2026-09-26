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

#include "Moose.h"
#include "metaphysicl/raw_type.h"

#include <type_traits>
#include <vector>

/**
 * Group spacing policy for grouped cluster tails.
 */
enum class ClusterGroupingSpacing
{
  LINEAR = 0,
  GEOMETRIC = 1
};

/**
 * Metadata for one grouped bin in the section-5 L0/L1 formulation.
 *
 * The physical cluster sizes covered by the bin are [start, end] inclusive.
 * The grouped state stores the two linear-in-bin coefficients L0 and L1 in the
 * array variable components l0_component and l1_component.
 */
struct ClusterGroupingBin
{
  unsigned int start = 0;
  unsigned int end = 0;
  unsigned int l0_component = libMesh::invalid_uint;
  unsigned int l1_component = libMesh::invalid_uint;
  Real width = 0.0;
  Real mean_x = 0.0;
  Real sigma2 = 0.0;
};

/**
 * Shared grouped-tail layout helper for cluster-dynamics array variables.
 *
 * Physical cluster size n is one-based:
 *   n = 1 -> monomer
 *   n = 2 -> dimer
 *   ...
 *
 * Array component layout:
 *   component 0 -> monomer
 *   components for 2 <= n <= explicit_max -> one component per explicit size
 *   grouped bins for n > explicit_max -> two components per bin (L0, L1)
 */
class ClusterGroupingLayout
{
public:
  ClusterGroupingLayout() = default;

  ClusterGroupingLayout(unsigned int num_cluster_sizes,
                        unsigned int explicit_max,
                        bool use_grouping,
                        ClusterGroupingSpacing spacing,
                        unsigned int linear_width,
                        Real geometric_ratio);

  void initialize(unsigned int num_cluster_sizes,
                  unsigned int explicit_max,
                  bool use_grouping,
                  ClusterGroupingSpacing spacing,
                  unsigned int linear_width,
                  Real geometric_ratio);

  unsigned int numClusterSizes() const { return _num_cluster_sizes; }
  unsigned int explicitMax() const { return _explicit_max; }
  bool useGrouping() const { return !_groups.empty(); }
  unsigned int componentCount() const { return _component_count; }
  unsigned int monomerComponent() const { return 0; }

  bool containsSize(unsigned int n) const;
  bool isExplicitSize(unsigned int n) const;
  bool isGroupedSize(unsigned int n) const;

  unsigned int explicitComponent(unsigned int n) const;
  int groupIndexForSize(unsigned int n) const;
  const ClusterGroupingBin & group(unsigned int group_index) const;
  const ClusterGroupingBin & groupForSize(unsigned int n) const;
  const std::vector<ClusterGroupingBin> & groups() const { return _groups; }

  /**
   * Coefficients of the in-bin reconstruction C_n = a0 * L0 + a1 * L1 for grouped size n.
   * When enforce_nonnegative is true, bins with L0 <= 0 reconstruct to zero and L1 is limited so
   * that the bin endpoint concentrations stay above -tolerance_factor * L0.
   */
  static void reconstructionCoefficients(const ClusterGroupingBin & bin,
                                         unsigned int n,
                                         Real l0,
                                         Real l1,
                                         bool enforce_nonnegative,
                                         Real tolerance_factor,
                                         Real & a0,
                                         Real & a1);

  /**
   * Concentration of physical cluster size n from the array state, reconstructing grouped sizes
   * from their bin's L0/L1 components.
   */
  template <typename Vector>
  auto concentration(const Vector & state,
                     unsigned int n,
                     bool enforce_nonnegative,
                     Real tolerance_factor) const
  {
    using Scalar = std::decay_t<decltype(state[0])>;
    if (isExplicitSize(n))
      return Scalar(state[explicitComponent(n)]);

    const auto & bin = groupForSize(n);
    const Scalar & l0 = state[bin.l0_component];
    const Scalar & l1 = state[bin.l1_component];
    Real a0, a1;
    reconstructionCoefficients(bin,
                               n,
                               MetaPhysicL::raw_value(l0),
                               MetaPhysicL::raw_value(l1),
                               enforce_nonnegative,
                               tolerance_factor,
                               a0,
                               a1);
    return Scalar(a0 * l0 + a1 * l1);
  }

private:
  static Real integerMean(unsigned int start, unsigned int end);
  static Real integerSigma2(unsigned int start, unsigned int end);

  unsigned int _num_cluster_sizes = 0;
  unsigned int _explicit_max = 0;
  unsigned int _component_count = 1;

  std::vector<unsigned int> _explicit_component;
  std::vector<int> _group_of_size;
  std::vector<ClusterGroupingBin> _groups;
};
