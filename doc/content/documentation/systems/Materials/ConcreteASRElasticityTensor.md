# Concrete ASR Elasticity Tensor

!syntax description /Materials/ConcreteASRElasticityTensor

## Description

The `ConcreteASRElasticityTensor` model computes an elasticity tensor that accounts for the effect of alkali-silica reaction (ASR) in degrading the stiffness according to the model of [cite:saouma_constitutive_2006]:
\begin{equation}
E(t, \theta) = E_0[1 - (1 - \beta_E)\xi(t, \theta)]
\end{equation}
where $E$ is the degraded elastic modulus, $E_0$ is the original elastic modulus, $\beta_E$ is the residual fractional value of the elastic modulus when the concrete has fully reacted, $t$ is the time, $\theta$ is the temperature, and $\xi$ is the extent of ASR reaction, which goes from 0 (no reaction) to 1 (fully reacted).  $\beta_E$ can be set using the `residual_youngs_modulus_fraction` input parameter.

## Implementation and Usage

This model only computes the modified elasticity tensor, and relies on the [ConcreteASREigenstrain](ConcreteASREigenstrain.md) model to compute the ASR reaction extent $\xi$, which is stored in a property named `ASR_extent`, so it must be used in conjunction with that model.

## Example Input Syntax

!listing test/tests/concrete_ASR_swelling/asr_confined.i block=Materials/elasticity_concrete

!syntax parameters /Materials/ConcreteASRElasticityTensor

!syntax inputs /Materials/ConcreteASRElasticityTensor

!syntax children /Materials/ConcreteASRElasticityTensor

!bibtex bibliography
