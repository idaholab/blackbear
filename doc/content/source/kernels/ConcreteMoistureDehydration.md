# ConcreteMoistureDehydration

!syntax description /Kernels/ConcreteMoistureDehydration

## Description

This kernel provides a source term accounting for dehydrated water in the governing equation for moisture in concrete. The amount of dehydrated water, $W_d$, in units of (g water)/(g material)/(cm$^3$ volume of material), represents the water molecules released into concrete pores due to dehydration of hydrated minerals within cement paste as the temperature in concrete increases. $W_d$ is given by the following empirical formula:

!equation id=Wd
W_d = W_H^{105}f_d(T),

in which $W_H^{105}$ is the hydrated water content (in units of (g water)/(g material)/(cm$^3$ volume of material)) at 105 $\degree$C, and $f_d$ is a function of the weight loss of the concrete due to heat. Dehydration of hydrated minerals within cement paste typically begins at about 120 $\degree$C.

The function describing variation of $W_H$ over time is given by

!equation id=WH
\begin{array}{lll}
W_H(t_e) = 0.21c(\frac{t_e}{\tau_e + t_e}), & \tau_e = 23 & \textrm{days}
\end{array}

where $c$ is mass of (anhydrous) cement per cm$^3$ of concrete and $t_e$ is the equivalent hydration period, which is given by

!equation id=teq
t_e = \int \beta_H\beta_Tdt \\
\beta_H(H) = \frac{1}{1+(3.5-3.5H)^4} \\
\beta_T(T) =  e^{\frac{Q_h}{R}(\frac{1}{T_{ref}}-\frac{1}{T})}

where

$t$ = actual time \\
$Q_h$ = activation energy of hydration \\
$\frac{Q_h}{R}$ = 2700 K

Typical values of $f_d(T)$ are plotted by [!cite](harmathy1973thermal). In this implementation, a simple linear function

!equation id=fdT
f_d(T) = \frac{0.05T}{600},

is used to account for mass loss of concrete due to dehydration. It should be noted that $W_d$ is often negligible in modeling moisture diffusion under most circumstances unless the concrete is subjected to rapid heating conditions at high temperature. It is also interesting to note that by extending the definition of $W_d$ to hydrated water, the same model can also be applied to simulate the moisture distribution in concrete during the curing process. In such case, $W_d$ becomes a sink term and represents the loss of free water molecules in pores due to hydration reactions.

The material properties used by this model are computed by the [ConcreteThermalMoisture](ConcreteThermalMoisture.md) material.

!syntax parameters /Kernels/ConcreteMoistureDehydration

!syntax inputs /Kernels/ConcreteMoistureDehydration

!syntax children /Kernels/ConcreteMoistureDehydration

!bibtex bibliography
