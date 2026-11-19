# SpatioTemporalPaths System

The SpatioTemporalPaths system defines spatio-temporal paths, hereinafter referred to as paths,
that are often useful in additive manufacturing and welding simulations.

## Overview

A path is a piecewise linear, vector-valued function of time, $\boldsymbol{x}(t)$, where
$\boldsymbol{x}$ is the path front at time $t$. In addition to the path front, the path provides
its direction of travel $\boldsymbol{d}$, the unit vector of the path's velocity:

\begin{equation}
  \boldsymbol{d} = \dfrac{\dot{\boldsymbol{x}}}{\lVert\dot{\boldsymbol{x}}\rVert}
\end{equation}

On each linear segment the velocity is constant, so $\boldsymbol{d}$ is exact and changes abruptly
at the vertices. See [SpatioTemporalPath](SpatioTemporalPath.md) for how the vertices are given.

## Example Input File Syntax

Spatio-temporal paths are defined under the `[SpatioTemporalPaths]` in the input file and can be referenced by other objects in the input file.

!syntax list /SpatioTemporalPaths objects=True actions=False subsystems=False

!syntax list /SpatioTemporalPaths objects=False actions=False subsystems=True

!syntax list /SpatioTemporalPaths objects=False actions=True subsystems=False
