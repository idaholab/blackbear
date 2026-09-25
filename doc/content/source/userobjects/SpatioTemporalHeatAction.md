# A Modular Action for Moving Heat Source Simulation

## Overview

The `SpatioTemporalHeatAction` is a custom MOOSE `Action` developed to streamline the setup of a spatio-temporal heat source. It automates the creation and coordination of four essential components for transient thermal simulations:

1. **UserObject: `CSVPiecewiseLinearSpatioTemporalPath`**
   Defines the time-dependent path of the moving heat source based on a CSV input file. This user object enables interpolation of the source location over time.

2. **MeshModifier: `SpatioTemporalPathElementSubdomainModifier`**
   Dynamically modifies the subdomain of elements within a specified distance from the moving path.

3. **Material: `ADMovingEllipsoidalHeatSource`**
   Provides an ellipsoidal volumetric heat source localized around the current position of the path.

4. **Kernel: `ADMatHeatSource`**
   Inserts the volumetric heat source term into the weak form of the governing heat equation, linking it to the temperature field variable.

By combining these components under one `Action`, we simplify user input by encapsulating path behavior, mesh modification, source material response, and kernel integration in a single configuration interface.


## Syntax

### `SpatioTemporalHeatAction` Parameters

!syntax parameters /Actions/SpatioTemporalHeatAction


## Example Usage

If the following configuration is provided in the input file:
```
[SpatioTemporalHeat]
  path_file = 'concentric_circles_reverse.csv'

  #for path
  verbose = true

  #for esm
  block = '0 1'
  radius = 0.03
  execute_on_esm = 'TIMESTEP_BEGIN'
  old_subdomain_reinitialized = false
  reinitialize_subdomains = '0'
  reinitialization_strategy = "POLYNOMIAL_NEIGHBOR"
  reinitialize_variables = "T"
  polynomial_fitters = 'extrapolation_patch_T '

  #for heat source
  power = 1
  a = 0.035
  b = 0.01
  efficiency = 1
  scale = 1

  #for kernel
  heat_variable = cond
[]
```

Then the `SpatioTemporalHeat` action will interpret this configuration and automatically generate the equivalent set of manual blocks shown below:
```
[SpatioTemporalPaths]
  [path]
  type = CSVPiecewiseLinearSpatioTemporalPath
  file = 'concentric_circles_reverse.csv'
  verbose = true
  []
[]

[MeshModifiers]
  [esm]
  type = SpatioTemporalPathElementSubdomainModifier
  path = 'path'
  radius = 0.03
  target_subdomain = '0'
  block = '0 1'
  execute_on = 'TIMESTEP_END'
  old_subdomain_reinitialized = false
  reinitialize_subdomains = '0'
  reinitialization_strategy = "POLYNOMIAL_NEIGHBOR"
  reinitialize_variables = "T"
  polynomial_fitters = 'extrapolation_patch_T '
  []
[]

[Materials]
  [volumetric_heat]
  type = ADMovingEllipsoidalHeatSource
  path = 'path'
  power = 1
  efficiency = 1
  scale = 1
  a = 0.035
  b = 0.01
  []
[]

[Kernels]
  [hsource]
  type = ADMatHeatSource
  material_property = 'volumetric_heat'
  variable = cond
  []
[]
```
