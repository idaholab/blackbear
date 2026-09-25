# ClusterTotalDensityGrouped

!syntax description /Postprocessors/ClusterTotalDensityGrouped

## Description

`ClusterTotalDensityGrouped` computes the number density represented by an
explicit and grouped cluster distribution:

!equation
\rho =
\frac{1}{V_{at}}\sum_{n=n_\mathrm{min}}^N C_n.

For a complete bin without reconstruction clipping, the centered $L_1$ term
sums to zero and the bin contribution is $\Delta L_0$. Other bins are summed
over their reconstructed physical concentrations. When `atomic_volume` is
provided in m$^3$, the result is reported in `#/m^3`.

The grouping layout and nonnegative-reconstruction parameters must match those
used by [ClusterDynamicsGroupedNodalKernel.md].

## Example Input Syntax

!listing test/tests/cluster_dynamics/cluster_dynamics_grouped_small.i block=Postprocessors/total_cluster_density

!syntax parameters /Postprocessors/ClusterTotalDensityGrouped

!syntax inputs /Postprocessors/ClusterTotalDensityGrouped

!syntax children /Postprocessors/ClusterTotalDensityGrouped
