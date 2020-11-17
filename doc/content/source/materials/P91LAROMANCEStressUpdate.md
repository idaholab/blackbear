# P91LAROMANCEStressUpdate

## Description

`P91LAROMANCEStressUpdate` implements the necessary coefficients to compute a creep rate for Grade 91
chrome moly alloy steel by sampling a Los Alamos Reduced Order Model Applied to Nonlinear Constitutive
Equations (LAROMANCE) model. `P91LAROMANCEStressUpdate` implements the coefficients required by
[ADLAROMANCEStressUpdateBase](LAROMANCE.md), which in turn utilizes the radial
return method implemented in [ADRadialReturnStressUpdate](/ADRadialReturnStressUpdate.md) to
compute a creep rate. The coefficients are formulated by many precomputed lower-length scale
simulations, and calibrated to Legendre polynomials. See
[ADLAROMANCEStressUpdateBase](LAROMANCE.md) for a more extensive review of the
model.

## Example Input Syntax

!listing test/tests/p91_rom/verification.i block=Materials/rom_stress_prediction

!syntax parameters /Materials/P91LAROMANCEStressUpdate

!syntax inputs /Materials/P91LAROMANCEStressUpdate

!syntax children /Materials/P91LAROMANCEStressUpdate

!bibtex bibliography
