# ConcreteLatentHeat

!syntax description /Kernels/ConcreteLatentHeat

## Description

This kernel provides the latent heat term in the governing partial differential equation for heat transfer in concrete given by [!cite](bazant1982finite) and [!cite](saouma_structural_2014) as

!equation id=latent_heat
C_a\frac{\partial{W}}{\partial{H}}\frac{\partial{H}}{\partial{t}}

where:

$T$   =  temperature in $\degree$C\\
$W$   =  water (moisture) content in g/g (for unit volume  of material, m$^3$)\\
$H$   =  pore relative humidity\\
$C_a$   =  heat absorption of free water in $J/kg$\\
$\frac{\partial{W}}{\partial{H}}$   =  moisture capacity in g/gm$^3$\\
$t$   =  time in $s$

!syntax parameters /Kernels/ConcreteLatentHeat

!syntax inputs /Kernels/ConcreteLatentHeat

!syntax children /Kernels/ConcreteLatentHeat

!bibtex bibliography
