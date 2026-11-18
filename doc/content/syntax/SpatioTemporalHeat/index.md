# SpatioTemporalHeat System

The `[SpatioTemporalHeat]` block provides a single, modular interface for setting up a moving heat source that follows a spatio-temporal path, which is commonly needed in additive manufacturing and welding simulations.

## Overview

Instead of manually defining and coordinating the individual objects required for a moving heat source, the `[SpatioTemporalHeat]` block lets the user configure everything in one place. The associated [`SpatioTemporalHeatAction`](SpatioTemporalHeatAction.md) then automatically creates and connects the following components:

- a [`CSVPiecewiseLinearSpatioTemporalPath`](CSVPiecewiseLinearSpatioTemporalPath.md) user object defining the time-dependent path,
- a [`SpatioTemporalPathElementSubdomainModifier`](SpatioTemporalPathElementSubdomainModifier.md) that activates elements along the path,
- an [`ADMovingEllipsoidalHeatSource`](ADMovingEllipsoidalHeatSource.md) material providing the volumetric heat source, and
- an `ADMatHeatSource` kernel inserting the heat source term into the heat equation.

See the [SpatioTemporalPath system](syntax/SpatioTemporalPaths/index.md) for more details on how the underlying paths are defined.

## Example Input File Syntax

The moving heat source is defined under the `[SpatioTemporalHeat]` block in the input file. See the [`SpatioTemporalHeatAction`](SpatioTemporalHeatAction.md) page for the full list of parameters and an example configuration.

!syntax list /SpatioTemporalHeat objects=True actions=False subsystems=False

!syntax list /SpatioTemporalHeat objects=False actions=False subsystems=True

!syntax list /SpatioTemporalHeat objects=False actions=True subsystems=False
