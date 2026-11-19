# SpatioTemporalPath

!syntax description /SpatioTemporalPaths/SpatioTemporalPath

## Description

`SpatioTemporalPath` defines a piecewise linear path $\boldsymbol{x}(t)$ through a set of
vertices. The vertices are given either directly with the `t`, `x`, `y`, and `z` parameters, or
from a CSV file (`file`) with columns named `t` and any of `x`, `y`, `z`. Coordinates that are
omitted are zero along the whole path. The times must be strictly increasing.

The position is linearly interpolated in time and held at the first or last vertex outside the
time range. The direction is the unit vector of the active segment, so it changes abruptly at a
vertex, and it is zero outside the time range. The position and direction are updated at the start
of every time step, before any object that uses the path is executed.

## Example Input File Syntax

!listing test/tests/spatiotemporal_path/manual.i block=SpatioTemporalPaths

!listing test/tests/spatiotemporal_path/csv.i block=SpatioTemporalPaths

!syntax parameters /SpatioTemporalPaths/SpatioTemporalPath

!syntax inputs /SpatioTemporalPaths/SpatioTemporalPath

!syntax children /SpatioTemporalPaths/SpatioTemporalPath
