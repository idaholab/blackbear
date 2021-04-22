# ConcreteLatentHeat

This kernel provides latent heat part in the governing partial differential equation for heat transfer in concrete is given by [!cite](bavzant1982finite) and [!cite](victoYann) as

!equation id=latent_heat
C_a\frac{\partial{W}}{\partial{H}}\frac{\partial{H}}{\partial{t}}

where:

$T$   =  temperature in $°$C\\
$W$   =  water (moisture) content in g/g (for unit volume  of material, m$^3$)\\
$H$   =  pore relative humidity\\
$C_a$   =  heat absorption of free water in $J/kg$\\
$\frac{\partial{W}}{\partial{H}}$   =  moisture capacity in g/gm$^3$\\
$t$   =  time in $s$

!syntax description /Kernels/ConcreteLatentHeat

!syntax parameters /Kernels/ConcreteLatentHeat

!syntax inputs /Kernels/ConcreteLatentHeat

!syntax children /Kernels/ConcreteLatentHeat

!bibtex bibliography
