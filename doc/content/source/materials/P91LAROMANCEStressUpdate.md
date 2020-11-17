# P91LAROMANCEStressUpdate

## Description

`P91LAROMANCEStressUpdate` implements the necessary coefficients to compute a creep rate for P91
chrome moly alloy steel by sampling a Los Alamos Reduced Order Model Applied to Nonlinear Constitutive
Equations (LAROMANCE) model. `P91LAROMANCEStressUpdate` implements the coefficients required by
[ADLAROMANCEStressUpdateBase](ADLAROMANCEStressUpdateBase.md), which in turn utilizes the radial
return method implemented in [ADRadialReturnStressUpdate](/ADRadialReturnStressUpdate.md) to
compute a creep rate. The coefficients are formulated by many precomputed lower-length scale
simulations, and calibrated to Legendre polynomials. See
ADLAROMANCEStressUpdateBase](ADLAROMANCEStressUpdateBase.md) for a more extensive review of the
model.

## Example Input Syntax

!listing test/tests/d91_rom/verification.i block=Materials/rom_stress_prediction

!syntax parameters /Materials/tensor_mechanics/P91LAROMANCEStressUpdate

!syntax inputs /Materials/tensor_mechanics/P91LAROMANCEStressUpdate

!syntax children /Materials/tensor_mechanics/P91LAROMANCEStressUpdate

!bibtex bibliography
