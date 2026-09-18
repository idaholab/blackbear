# SingleNodeMesh

!syntax description /Mesh/SingleNodeMesh

## Description

The `SingleNodeMesh` creates a mesh with a single node and a single +NODEELEM+ element, suitable for zero-dimensional (point) nodal simulations such as a 0-D cluster dynamics problem using [ClusterDynamicsNodalKernel.md].

!alert note title=Nodal physics only
`SingleNodeMesh` is intended for problems that use only [NodalKernels](syntax/NodalKernels/index.md).  No element-level integration is performed on a NODEELEM, so the mesh is not suitable for kernels or materials that require quadrature point evaluation.

!syntax parameters /Mesh/SingleNodeMesh

!syntax inputs /Mesh/SingleNodeMesh

!syntax children /Mesh/SingleNodeMesh

## See Also

- [ClusterDynamicsNodalKernel.md] - Cluster dynamics array nodal kernel
- [ClusterAverageRadius.md] - Average cluster radius postprocessor
- [ClusterTotalDensity.md] - Total cluster density postprocessor
- [ClusterSizeConcentration.md] - Single cluster size concentration postprocessor
