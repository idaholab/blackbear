# ClusterSizeConcentrationGrouped

!syntax description /Postprocessors/ClusterSizeConcentrationGrouped

## Description

`ClusterSizeConcentrationGrouped` reports the concentration of one physical
cluster size. An explicit concentration is read directly. A grouped
concentration is reconstructed as

!equation
C_n = L_0 + L_1(n-\bar n),

using the bin that contains
[!param](/Postprocessors/ClusterSizeConcentrationGrouped/n_size). The grouping
layout and nonnegative-reconstruction parameters must match those used by
[ClusterDynamicsGroupedNodalKernel.md].

## Example Input Syntax

!listing test/tests/cluster_dynamics/cluster_dynamics_grouped_small.i block=Postprocessors/monomer_concentration

!syntax parameters /Postprocessors/ClusterSizeConcentrationGrouped

!syntax inputs /Postprocessors/ClusterSizeConcentrationGrouped

!syntax children /Postprocessors/ClusterSizeConcentrationGrouped
