# ConcreteThermalConduction

!syntax description /Kernels/ConcreteThermalConduction

## Description

This kernel provides thermal conductivity part in the governing partial differential equation for heat transfer in concrete is given by [!cite](bazant1982finite) and [!cite](saouma_structural_2014) as

!equation id=thermal_conduction
\nabla \cdot (k \nabla T)

where:

$T$   =  temperature in $\degree$C\\
$k$   =  thermal conductivity of concrete in W/(m$\degree$C)

The thermal conductivity used by this model is computed by the [ConcreteThermalMoisture](ConcreteThermalMoisture.md) material.

!syntax parameters /Kernels/ConcreteThermalConduction

!syntax inputs /Kernels/ConcreteThermalConduction

!syntax children /Kernels/ConcreteThermalConduction

!bibtex bibliography
