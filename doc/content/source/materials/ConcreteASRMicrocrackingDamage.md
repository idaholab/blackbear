# Concrete ASR Microcracking Damage

!syntax description /Materials/ConcreteASRMicrocrackingDamage

## Description

The `ConcreteASRMicrocrackingDamage` model computes a scalar damage index that increases with the extent of alkali-silica reaction (ASR) according to the model of [cite:saouma_constitutive_2006]:
\begin{equation}
d^{ASR}(t, \theta) = (1 - \beta_E)\xi^{ASR}(t, \theta)
\end{equation}
where $d^{ASR}$ is the damage induced by the ASR, $\beta_E$ is the residual fractional value of the elastic modulus when the concrete has fully reacted, $t$ is the time, $\theta$ is the temperature, and $\xi^{ASR}$ is the extent of ASR reaction, which goes from 0 (no reaction) to 1 (fully reacted).  $\beta_E$ can be set using the `residual_youngs_modulus_fraction` input parameter.

## Implementation and Usage

This model only computes a scalar damage index, and relies on the [ConcreteASREigenstrain](ConcreteASREigenstrain.md) model to compute the ASR reaction extent $\xi^{ASR}$, which is stored in a property named `ASR_extent`, so it must be used in conjunction with that model.
In order for the damage to be applied, the stress needs to be computed using a [ComputeDamageStress](ComputeDamageStress.md) stress calculator.

## Example Input Syntax

!listing test/tests/concrete_ASR_swelling/asr_confined.i block=Materials/ASR_damage_concrete

!syntax parameters /Materials/ConcreteASRMicrocrackingDamage

!syntax inputs /Materials/ConcreteASRMicrocrackingDamage

!syntax children /Materials/ConcreteASRMicrocrackingDamage

!bibtex bibliography
