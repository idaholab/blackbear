# ConcreteMoistureDiffusion

This kernel implement a part of the governing equation ([!cite](victoYann)) for moisture diffusion in concrete is formulated by using relative humidity $H$ as the primary variable:

!equation id=ConcreteMoistureDiffusion
\nabla (D_h\nabla H) + \nabla (D_{ht}\nabla T) + \frac{\partial{W_d}}{\partial{t}},

where

$H$ = pore relative humidity, and $H=P_v$/$P_{vs}$ \\
$P_{vs}$ = saturate vapor pressure $= P_{atm}e^{\left(4871.3\frac{T-100}{373.15T}\right)}$\\
      ( Bary and Poryet (2012) ($T$ is the temperature in K))\\
$P_{atm}$ = standard atmospheric pressure $= 101.325 Pa$ \\
$D_h$ = moisture diffusivity (also referred as humidity diffusivity) (cm$^2$/day)\\
$D_{ht}$= coupled moisture diffusivity under the influence of a temperature gradient in cm$^2$/day \\
$W_d$= total mass of free evaporable water released into the pores\\
   by dehydration of the cement paste\\
$t$  = time $(day)$

The moisture diffusivity $D_h$ depends on the relative humidity $H$. Thus the moisture diffusion
governing equation is highly nonlinear. The following sections describes in detail the constitutive models for this two parameters. Also it is worth noting that since the
relative humidity strongly depends on the temperature $T$, the moisture diffusion model in Grizzly is always coupled with heat transfer model.

!syntax description /Kernels/ConcreteMoistureDiffusion

!syntax parameters /Kernels/ConcreteMoistureDiffusion

!syntax inputs /Kernels/ConcreteMoistureDiffusion

!syntax children /Kernels/ConcreteMoistureDiffusion

!bibtex bibliography
