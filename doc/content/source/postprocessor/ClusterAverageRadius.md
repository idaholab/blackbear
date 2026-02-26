# ClusterAverageRadius

!syntax description /Postprocessors/ClusterAverageRadius

## Description

The `ClusterAverageRadius` postprocessor computes the volume-weighted average cluster radius from the cluster dynamics array nodal variable used by [ClusterDynamicsNodalKernel.md] on the domain or within a subdomain.

Only cluster sizes $n \geq n_\text{min}$ ([!param](/Postprocessors/ClusterAverageRadius/n_minimum)) are included; by default $n_\text{min} = 2$, which excludes the monomer.

### Calculation

The average cluster radius at a single node is given by:

!equation
\langle r \rangle = \frac{\displaystyle\sum_{n=n_\text{min}}^{N} r_n \, C_n}{\displaystyle\sum_{n=n_\text{min}}^{N} C_n}

where $r_n = r_1 n^{1/3}$ is the radius of a cluster of size $n$ with $r_1$ ([!param](/Postprocessors/ClusterAverageRadius/r1)) being the monomer radius, $C_n$ ([!param](/Postprocessors/ClusterAverageRadius/clusters)) is its concentration, and $N$ is the total number of cluster sizes in the array variable.  $\langle r \rangle$ is the average over all nodes in the domain or subdomain.

## Example Input Syntax

!listing test/tests/nodalkernels/cluster_dynamics/cluster_dynamics_50_combined.i block=Postprocessors/avg_cluster_radius

!syntax parameters /Postprocessors/ClusterAverageRadius

!syntax inputs /Postprocessors/ClusterAverageRadius

!syntax children /Postprocessors/ClusterAverageRadius

## See Also

- [ClusterDynamicsNodalKernel.md] - Cluster dynamics array nodal kernel
- [ClusterTotalDensity.md] - Total cluster density postprocessor
- [ClusterSizeConcentration.md] - Single cluster size concentration postprocessor
