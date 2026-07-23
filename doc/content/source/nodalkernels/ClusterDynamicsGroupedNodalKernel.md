# ClusterDynamicsGroupedNodalKernel

!syntax description /NodalKernels/ClusterDynamicsGroupedNodalKernel

## Description

`ClusterDynamicsGroupedNodalKernel` solves the same one-species cluster-dynamics
system as [ClusterDynamicsNodalKernel.md], but reduces the number of array
components used for large clusters. Cluster sizes through
[!param](/NodalKernels/ClusterDynamicsGroupedNodalKernel/explicit_max) are
stored explicitly. The remaining physical sizes are divided into bins, and two
moments are stored for each bin.

The matching [GroupedVariables](syntax/GroupedVariables/index.md) block
computes the required array component count. The user therefore specifies the
maximum physical cluster size and grouping controls, but does not calculate
`components`.

## Grouped Representation

For a bin containing physical cluster sizes $n=a,\ldots,b$, define

!equation
\Delta = b-a+1, \qquad
\bar n = \frac{1}{\Delta}\sum_{n=a}^{b} n, \qquad
\sigma_n^2 = \frac{1}{\Delta}\sum_{n=a}^{b}(n-\bar n)^2.

The stored zeroth and first coefficients are

!equation
L_0 = \frac{1}{\Delta}\sum_{n=a}^{b} C_n,
\qquad
L_1 = \frac{1}{\Delta\sigma_n^2}\sum_{n=a}^{b}(n-\bar n)C_n.

The concentration within the bin is reconstructed as

!equation
C_n \simeq L_0 + L_1(n-\bar n).

The `linear` spacing option uses a fixed bin width set by
[!param](/NodalKernels/ClusterDynamicsGroupedNodalKernel/group_linear_width).
The `geometric` option starts with that width and increases each subsequent
width using
[!param](/NodalKernels/ClusterDynamicsGroupedNodalKernel/group_geometric_ratio).
The final bin is shortened when it reaches
[!param](/NodalKernels/ClusterDynamicsGroupedNodalKernel/num_cluster_sizes).

## Moment Evolution

The boundary flux between physical sizes $n$ and $n+1$ is

!equation
J_n = \beta_n C_1 C_n - \alpha_{n+1}C_{n+1},
\qquad J_N=0.

The grouped zeroth coefficient follows directly from the two bin-boundary
fluxes:

!equation
\frac{dL_0}{dt} = \frac{J_{a-1}-J_b}{\Delta}.

For $\Delta>1$, define the average internal boundary flux

!equation
\overline J =
\frac{1}{\Delta-1}\sum_{n=a}^{b-1}J_n.

The corrected first-coefficient equation is

!equation
\frac{dL_1}{dt} =
-\frac{\Delta-1}{2\sigma_n^2\Delta}
\left(J_{a-1}-2\overline J+J_b\right).

These equations preserve the flux form at both group boundaries. The monomer
equation uses the reconstructed physical fluxes, including the factor of two
for dimer formation. The upper boundary is closed at the largest represented
cluster size.

## Reconstruction Limiter

With
[!param](/NodalKernels/ClusterDynamicsGroupedNodalKernel/enforce_group_nonnegative)
set to `true`, the reconstructed slope is limited so that concentrations at
the two bin endpoints do not fall below the permitted tolerance. A value of
zero for
[!param](/NodalKernels/ClusterDynamicsGroupedNodalKernel/group_nonnegative_tolerance_factor)
enforces nonnegative endpoints. A positive value permits an endpoint to reach
`-factor * L0` before limiting. Setting `enforce_group_nonnegative = false`
uses the unmodified linear reconstruction.

The limiter is useful when a wide bin develops a steep first moment, but it
also introduces a nonsmooth residual. Group widths should therefore be chosen
with both accuracy and nonlinear convergence in mind.

## Rate Models

The grouped and ungrouped kernels provide the same `simple` and
`interfacial_energy` rate models. They also provide the same
`interfacial_energy`, `capillary`, `binding_energy_table`, and
`formation_energy_table` binding-energy options. The equations, units, and
input examples are documented in [ClusterDynamicsNodalKernel.md].

The
[!param](/NodalKernels/ClusterDynamicsGroupedNodalKernel/radiation_enhanced_factor)
parameter applies a fixed multiplicative factor to the monomer diffusivity.
Evolution of point-defect concentrations is not part of this kernel.

## Disabling Grouping

Setting `use_grouping = false` makes every physical cluster size explicit
while retaining the grouped interface. In that mode the number of components
equals `num_cluster_sizes`, and the residual follows the ungrouped master
equations.

## Example Input Syntax

!listing test/tests/cluster_dynamics/cluster_dynamics_grouped_small.i

!syntax parameters /NodalKernels/ClusterDynamicsGroupedNodalKernel

!syntax inputs /NodalKernels/ClusterDynamicsGroupedNodalKernel

!syntax children /NodalKernels/ClusterDynamicsGroupedNodalKernel

## See Also

- [ClusterDynamicsNodalKernel.md]
- [GroupedArrayFunctionIndexIC.md]
- [ClusterAverageRadiusGrouped.md]
- [ClusterTotalDensityGrouped.md]
- [ClusterSizeConcentrationGrouped.md]
