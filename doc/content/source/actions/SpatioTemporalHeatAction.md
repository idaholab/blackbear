# SpatioTemporalHeatAction

!syntax description /SpatioTemporalHeat/SpatioTemporalHeatAction

## Description

The `SpatioTemporalHeatAction` sets up a moving heat source from a single
[SpatioTemporalHeat](syntax/SpatioTemporalHeat/index.md) block. The path is defined in the
`[path]` sub-block as a [SpatioTemporalPath](SpatioTemporalPath.md). The action then creates:

1. a [SpatioTemporalPathElementSubdomainModifier](SpatioTemporalPathElementSubdomainModifier.md)
   that moves elements near the path front into `target_subdomain`,
2. an [ADMovingEllipsoidalHeatSource](ADMovingEllipsoidalHeatSource.md) material that follows the
   path, and
3. an `ADMatHeatSource` kernel that applies the heat source to `heat_variable`.

The generated objects are named after the block (`SpatioTemporalHeat_path`,
`SpatioTemporalHeat_subdomain_modifier`, `SpatioTemporalHeat_heat_source`,
`SpatioTemporalHeat_volumetric_heat`, and `SpatioTemporalHeat_heat_kernel`) to avoid collisions
with user-defined objects. The subdomain modifier and heat source parameters are the same as those
of the underlying objects; `execute_on_esm` sets the subdomain modifier's `execute_on`.

## Example Input File Syntax

!listing test/tests/spatiotemporal_path/esm_hsource.i block=SpatioTemporalHeat

!syntax parameters /SpatioTemporalHeat/SpatioTemporalHeatAction
