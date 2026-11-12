# GroupedClusterVariableAction

!syntax description /GroupedVariables/GroupedClusterVariableAction

## Description

The `GroupedClusterVariableAction` is a MOOSE action that adds the array variable used by
[GenericClusterDynamicsGroupedNodalKernel.md]. It builds the explicit and grouped cluster layout from
the grouping parameters and sets the number of array components automatically. This is invoked using
the [GroupedVariables](syntax/GroupedVariables/index.md) block. See that page for a description,
example usage, and parameters.
