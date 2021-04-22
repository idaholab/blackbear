# ConcreteMoistureTimeIntegration

This kernel implement a part of the governing equation ([!cite](victoYann)) for moisture diffusion in concrete is formulated by using relative humidity $H$ as the primary variable:

!equation id=moisture_governing
\frac{\partial{W}}{\partial{H}} \frac{\partial{H}}{\partial{t}}

where

$W$ = total water content (g/g) (for unit volume of concrete, cm$^3$) \\
$H$ = pore relative humidity, and $H=P_v$/$P_{vs}$ \\
$P_{vs}$ = saturate vapor pressure $= P_{atm}e^{\left(4871.3\frac{T-100}{373.15T}\right)}$\\
      ( Bary and Poryet (2012) ($T$ is the temperature in K))\\
$P_{atm}$ = standard atmospheric pressure $= 101.325 Pa$ \\
$t$  = time $(day)$

The moisture capacity ${\partial W}/{\partial H}$ depends on the relative humidity $H$. Thus the moisture diffusion governing equation is highly nonlinear. The following sections describes in detail the constitutive models for this two parameters. Also it is worth noting that since the
relative humidity strongly depends on the temperature $T$, the moisture diffusion model in Grizzly is always coupled with heat transfer model.


!syntax description /Kernels/ConcreteMoistureTimeIntegration

!syntax parameters /Kernels/ConcreteMoistureTimeIntegration

!syntax inputs /Kernels/ConcreteMoistureTimeIntegration

!syntax children /Kernels/ConcreteMoistureTimeIntegration

!bibtex bibliography
