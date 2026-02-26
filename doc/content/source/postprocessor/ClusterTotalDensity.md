# ClusterTotalDensity

!syntax description /Postprocessors/ClusterTotalDensity

## Description

The `ClusterTotalDensity` postprocessor computes the total number density of clusters from the cluster dynamics array nodal variable ([!param](/Postprocessors/ClusterTotalDensity/clusters)) used by [ClusterDynamicsNodalKernel.md] on the domain or within a subdomain. Only cluster sizes $n \geq$ [!param](/Postprocessors/ClusterTotalDensity/n_minimum) are included, and the result is divided by the atomic volume $\Omega_a$ ([!param](/Postprocessors/ClusterTotalDensity/atomic_volume)).

### Calculation

!equation
\rho = \frac{1}{\Omega_a} \sum_{n=n_\text{min}}^{N} C_n

where $C_n$ is the concentration of clusters of size $n$, $N$ is the total number of cluster sizes, and $\Omega_a$ is the atomic volume. By default $n_\text{min} = 1$, which includes monomers.  $\rho$ is the average over all nodes in the domain or subdomain.

## Example Input Syntax

!listing test/tests/nodalkernels/cluster_dynamics/cluster_dynamics_50_combined.i block=Postprocessors/total_cluster_density

!syntax parameters /Postprocessors/ClusterTotalDensity

!syntax inputs /Postprocessors/ClusterTotalDensity

!syntax children /Postprocessors/ClusterTotalDensity

## See Also

- [ClusterDynamicsNodalKernel.md] - Cluster dynamics array nodal kernel
- [ClusterAverageRadius.md] - Average cluster radius postprocessor
- [ClusterSizeConcentration.md] - Single cluster size concentration postprocessor
