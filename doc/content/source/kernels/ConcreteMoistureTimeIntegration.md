# ConcreteMoistureTimeIntegration

!syntax description /Kernels/ConcreteMoistureTimeIntegration

## Description

This kernel implements the following time-dependent term in the governing equation ([!cite](saouma_structural_2014)) for moisture diffusion in concrete, which is formulated using relative humidity $H$ as the primary variable:

!equation id=moisture_governing
\frac{\partial{W}}{\partial{H}} \frac{\partial{H}}{\partial{t}}

where

$W$ = total water content (g/g) (for unit volume of concrete, cm$^3$) \\
$H$ = pore relative humidity \\
$t$  = time $(day)$

This kernel does not use mass lumping. A version of this kernel that uses mass lumping is provided in [ConcreteMoistureTimeIntegration](ConcreteMoistureTimeIntegration.md), and is generally recommended to minimize osciliatory behavior.

The moisture capacity ${\partial W}/{\partial H}$ depends on the relative humidity $H$. Thus the moisture diffusion governing equation is highly nonlinear. Also, it is worth noting that since the relative humidity strongly depends on the temperature $T$, the moisture diffusion model is always coupled with the heat transfer model. The moisture capacity is computed by the [ConcreteThermalMoisture](ConcreteThermalMoisture.md) material for the models that are formulated to provide it. For those that do not provide it, it is assumed to be equal to 1.

!syntax parameters /Kernels/ConcreteMoistureTimeIntegration

!syntax inputs /Kernels/ConcreteMoistureTimeIntegration

!syntax children /Kernels/ConcreteMoistureTimeIntegration

!bibtex bibliography
