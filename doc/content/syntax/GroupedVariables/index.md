# GroupedVariables

!syntax description /GroupedVariables/GroupedClusterVariableAction

## Description

The `GroupedVariables` block adds an array variable for an explicit and grouped
cluster distribution. It constructs the same layout used by
[ClusterDynamicsGroupedNodalKernel.md] and sets the array component count
automatically.

The array contains one monomer component, one component for each explicit size
from 2 through `explicit_max`, and two components for every grouped bin. The
two grouped components store $L_0$ and $L_1$. If `use_grouping = false`, every
physical cluster size is stored explicitly.

The layout parameters are commonly placed in `[GlobalParams]` so that the
variable action, initial condition, kernel, and grouped postprocessors all use
one consistent definition.

## Example Input Syntax

!listing test/tests/cluster_dynamics/cluster_dynamics_grouped_small.i block=GlobalParams GroupedVariables

!syntax parameters /GroupedVariables/GroupedClusterVariableAction

!syntax inputs /GroupedVariables/GroupedClusterVariableAction
