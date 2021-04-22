# ConcreteThermalConduction

This kernel provides thermal conductivity part in the governing partial differential equation for heat transfer in concrete is given by [!cite](bavzant1982finite) and [!cite](victoYann) as

!equation id=thermal_conduction
\frac{\partial}{\partial x} \left(k\frac{\partial T}{\partial x}\right) +
\frac{\partial}{\partial y} \left(k\frac{\partial T}{\partial y}\right)+
\frac{\partial}{\partial z} \left(k\frac{\partial T}{\partial z}\right)

where:

$T$   =  temperature in $°$C\\
$k$   =  thermal conductivity of concrete in W/m$°$C

!syntax description /Kernels/ConcreteThermalConduction

!syntax parameters /Kernels/ConcreteThermalConduction

!syntax inputs /Kernels/ConcreteThermalConduction

!syntax children /Kernels/ConcreteThermalConduction

!bibtex bibliography
