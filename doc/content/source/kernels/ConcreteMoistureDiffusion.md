# ConcreteMoistureDiffusion

!syntax description /Kernels/ConcreteMoistureDiffusion

## Description

This kernel provides the Fickian and Soret diffusion terms in the governing equation ([!cite](saouma_structural_2014)) for moisture diffusion in concrete, formulated using relative humidity $H$ as the primary variable. The Fickian term is dependent on the gradient in $H$:

!equation id=ConcreteMoistureDiffusionFickian
\nabla (D_h\nabla H)

while the Soret term is dependent on the gradient in temperature, $T$:

!equation id=ConcreteMoistureDiffusionSoret
\nabla (D_{ht}\nabla T)

where

$D_h$ = moisture diffusivity (also referred as humidity diffusivity) (cm$^2$/day)\\
$D_{ht}$= coupled moisture diffusivity under the influence of a temperature gradient in cm$^2$/day

The moisture diffusivity $D_h$ depends on the relative humidity $H$. Thus the moisture diffusion governing equation is highly nonlinear. Also it is worth noting that since the relative humidity strongly depends on the temperature $T$, the moisture diffusion model is always coupled with heat transfer model. These diffusivities are computed by the [ConcreteThermalMoisture](ConcreteThermalMoisture.md) material.

!syntax parameters /Kernels/ConcreteMoistureDiffusion

!syntax inputs /Kernels/ConcreteMoistureDiffusion

!syntax children /Kernels/ConcreteMoistureDiffusion

!bibtex bibliography
