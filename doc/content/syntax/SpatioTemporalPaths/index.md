# SpatioTemporalPath System

The SpatioTemporalPath system offers flexible ways of defining spatio-temporal paths, hereinafter referred to as paths, that are often useful in additive manufacturing simulations.

## Overview

A spatio-temporal path is a vector-valued function of time, written as

\begin{equation}
  \begin{aligned}
    \boldsymbol{x} = f(t)
  \end{aligned}
\end{equation}

where $\boldsymbol{x}$ is the path front at time $t$. In addition to the path front, the path object also defines the path's moving velocity $\boldsymbol{v}$ and the path's moving direction $\boldsymbol{t}$, defined as

\begin{equation}
  \begin{aligned}
    \boldsymbol{v} &= \dfrac{\boldsymbol{x}}{t}
    \boldsymbol{t} &= \dfrac{\boldsymbol{v}}{\lVert\boldsymbol{v}\rVert}
  \end{aligned}
\end{equation}

## Example Input File Syntax

Spatio-temporal paths are defined under the `[SpatioTemporalPaths]` in the input file and can be referenced by other objects in the input file.

!syntax list /SpatioTemporalPath objects=True actions=False subsystems=False

!syntax list /SpatioTemporalPath objects=False actions=False subsystems=True

!syntax list /SpatioTemporalPath objects=False actions=True subsystems=False
