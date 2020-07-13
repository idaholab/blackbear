<!-- MOOSE Documentation Stub: Remove this when content is added. -->

# ConcreteThermalConvection

!syntax description /Kernels/ConcreteThermalConvection

This kernel is part of the coupled heat transfer and moisture diffusion equation in concrete. This implements the following residual term representing the convective transport of heat due to fluid flow in concrete:

\begin{equation}
    R_T = C_w \, D_h \, \nabla H \, \nabla T
\end{equation}

Here, $T$ represents the temperature, $H$ is pore water relative humidity,
$C_w$ is mass density and isobaric (constant pressure) heat capacity of liquid water, and $D_h$ is the moisture diffusivity.

!syntax parameters /Kernels/ConcreteThermalConvection

!syntax inputs /Kernels/ConcreteThermalConvection

!syntax children /Kernels/ConcreteThermalConvection

!bibtex bibliography
