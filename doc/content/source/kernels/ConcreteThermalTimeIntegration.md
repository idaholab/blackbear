# ConcreteThermalTimeIntegration

This kernel utilizes thermal capacity term in the heat diffusion as per the governing partial differential equation for heat transfer in concrete is given by [!cite](bavzant1982finite) and [!cite](victoYann) as

!equation id=thermal_governing
\rho C\frac{\partial{T}}{\partial{t}}

where:

$\rho$  = density in kg/m$^3$ \\
$C$   =  specific heat of concrete in J/kg$°$C\\
$T$   =  temperature in $°$C\\
$t$   =  time in $s$

!syntax description /Kernels/ConcreteThermalTimeIntegration

!syntax parameters /Kernels/ConcreteThermalTimeIntegration

!syntax inputs /Kernels/ConcreteThermalTimeIntegration

!syntax children /Kernels/ConcreteThermalTimeIntegration

!bibtex bibliography
