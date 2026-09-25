# GroupedArrayFunctionIndexIC

!syntax description /ICs/GroupedArrayFunctionIndexIC

## Description

`GroupedArrayFunctionIndexIC` initializes a cluster distribution represented by
the layout created through the [GroupedVariables](syntax/GroupedVariables/index.md)
block. The supplied function is evaluated once for every physical cluster size.
For cluster size $n$, the function is evaluated at `x = n - 1`, consistent with
the zero-based convention used by [ArrayFunctionIndexIC.md].

Explicit concentrations are stored directly. For each grouped bin, the object
computes the $L_0$ and $L_1$ coefficients defined in
[ClusterDynamicsGroupedNodalKernel.md]. This preserves the zeroth and first
moments of the initial physical distribution within each bin.

## Example Input Syntax

!listing test/tests/cluster_dynamics/cluster_dynamics_grouped_small.i block=Functions ICs

!syntax parameters /ICs/GroupedArrayFunctionIndexIC

!syntax inputs /ICs/GroupedArrayFunctionIndexIC

!syntax children /ICs/GroupedArrayFunctionIndexIC
