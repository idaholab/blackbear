# ConcreteThermalTimeIntegration

!syntax description /Kernels/ConcreteThermalTimeIntegration

## Description

This kernel provides the time derivative (thermal capacity) term in the governing partial differential equation for heat transfer in concrete given by [!cite](bazant1982finite) and [!cite](saouma_structural_2014) as

!equation id=thermal_governing
\rho C\frac{\partial{T}}{\partial{t}}

where:

$\rho$  = density in kg/m$^3$ \\
$C$   =  specific heat of concrete in J/kg$\degree$C\\
$T$   =  temperature in $\degree$C\\
$t$   =  time in $s$

!syntax parameters /Kernels/ConcreteThermalTimeIntegration

!syntax inputs /Kernels/ConcreteThermalTimeIntegration

!syntax children /Kernels/ConcreteThermalTimeIntegration

!bibtex bibliography
