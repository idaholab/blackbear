# ConcreteThermalConvection

!syntax description /Kernels/ConcreteThermalConvection

## Description

This kernel provides the following convective transport term for thermal transport due to fluid flow in concrete:

\begin{equation}
    C_w\, D_h\, \nabla H\, \nabla T
\end{equation}

where:

$T$ =  temperature in $\degree$C\\
$H$ =  pore water relative humidity\\
$C_w$ = mass density and isobaric (constant pressure) heat capacity of liquid water\\
$D_h$ = moisture diffusivity (also referred as humidity diffusivity) (cm$^2$/day)\\

The material properties used by this model are computed by the [ConcreteThermalMoisture](ConcreteThermalMoisture.md) material.

!syntax parameters /Kernels/ConcreteThermalConvection

!syntax inputs /Kernels/ConcreteThermalConvection

!syntax children /Kernels/ConcreteThermalConvection

!bibtex bibliography
