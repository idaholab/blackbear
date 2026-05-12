# ClusterSizeConcentration

!syntax description /Postprocessors/ClusterSizeConcentration

## Description

The `ClusterSizeConcentration` postprocessor computes the volume-averaged concentration of a single cluster size from the cluster dynamics array nodal variable ([!param](/Postprocessors/ClusterSizeConcentration/clusters)) used by [ClusterDynamicsNodalKernel.md] on the domain or within a subdomain. The cluster size is specified via [!param](/Postprocessors/ClusterSizeConcentration/n_size); by default `n_size = 1`, which reads the monomer at array component index 0.

### Calculation

!equation
\bar{C}_{n} = \frac{1}{N_\text{nodes}} \sum_{j=1}^{N_\text{nodes}} C_{n,j}

where $n$ is the cluster size set by [!param](/Postprocessors/ClusterSizeConcentration/n_size), $C_{n,j}$ is the concentration of cluster size $n$ at node $j$ of the [!param](/Postprocessors/ClusterSizeConcentration/clusters) array variable), and $N_\text{nodes}$ is the total number of nodes in the domain or subdomain.

## Example Input Syntax

!listing test/tests/cluster_dynamics/cluster_dynamics_50_combined.i block=Postprocessors/monomer_concentration

!syntax parameters /Postprocessors/ClusterSizeConcentration

!syntax inputs /Postprocessors/ClusterSizeConcentration

!syntax children /Postprocessors/ClusterSizeConcentration

## See Also

- [ClusterDynamicsNodalKernel.md] - Cluster dynamics array nodal kernel
- [ClusterAverageRadius.md] - Average cluster radius postprocessor
- [ClusterTotalDensity.md] - Total cluster density postprocessor
