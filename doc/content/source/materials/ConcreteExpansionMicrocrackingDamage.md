# Concrete Expansion Microcracking Damage

!syntax description /Materials/ConcreteExpansionMicrocrackingDamage

## Description

The `ConcreteExpansionMicrocrackingDamage` model computes a scalar damage index $d$ that increases with the progress of the eigenstrain defined by the `eigenstrain_name` input parameter (for example, the eigenstrain induced by alkali-silica reaction (ASR) using a [ConcreteASREigenstrain](ConcreteASREigenstrain.md) model) and the current stress state of the material.

More specifically, this model is aimed at capturing anisotropic effects on ASR (and similar expansion mechanisms) without relying on an explicit redistribution of the eigenstrain itself.
Instead, external stresses increase the damage so that the \emph{apparent} expansion is negated in the direction of the load.
However, the eigenstrain still applies pressure in all directions.
This model uses the following assumptions:

- The eigenstrain is isotropic.
- In unconfined condition, the damage increases with the expansion.
- When the compressive stress exceeds a predefined value $\sigma_u$, the damage increases with the expansion so that the apaprent expansion is negated in the direction of the load.

In the following, $\varepsilon_{lin}^{eig}$ is the linear eigenstrain and $\sigma_c$ the compressive stress (measured as the sum of the negative components of the principal stress).

### Unconfined Damage

In unconfined condition $(\sigma_c = 0)$, the damage $d_{unconfined}$ depends only on the eigenstrain:

\begin{equation}
\label{eqn:damage_unconfined}
d_{unconfined} = 1.0 - \frac{\varepsilon_{branch}}{\varepsilon_{lin}^{eig} - (\varepsilon_{branch} + \varepsilon_{init})}
\end{equation}

In which $\varepsilon_{init}$ is the linear expansion at which damage initiates and $\varepsilon_{branch}$ controls the rate at which damage increases with the eigenstrain.

### Confined Damage

In confined condition (when $\sigma_c > \sigma_u$), the damage is further increased so that the apparent expansion in the direction of the load
The apparent expansion is defined by the difference between the current total strain (accounting for both the current damage and the eigenstrain itself), and the total strain that would be induced by the same stress on a pristine material (without expansion or damage).
The damage $d_{confined}$ is then obtained as:

\begin{equation}
\label{eqn:damage_confined}
d_{confined} = 1.0 - \frac{1.0}{1.0 + \frac{\mathrm{E} \varepsilon_{lin}^{eig}}{\sigma_c}}
\end{equation}

In which $\mathrm{E}$ is the Young's modulus of the pristine concrete.

The effect of stress on the damage can be disabled by setting the `use_stress_control` input parameter to `false`.

### Mixed Conditions

When $\sigma_c$ is between $0$ and $\sigma_u$, the overall damage is taken as a linear combination of the increment of unconfined and confined damage:

\begin{equation}
\label{eqn:damage_combined}
d = \left( 1 - \frac{\sigma_c}{\sigma_u} \right) d_{unconfined} + \frac{\sigma_c}{\sigma_u} d_{confined}
\end{equation}

### Anisotropic Eigenstrain

This model can accomodate for anisotropic eigenstrain by setting the `use_isotropic_expansion` input parameter to `false`, even though doing so is not recommended.
In this condition, $\varepsilon_{lin}^{eig}$ is defined as the maximum principal component of the eigenstrain.

## Implementation and Usage

This model only computes a scalar damage index, and must rely on an external model to compute the expansion.
If a [ConcreteASREigenstrain](ConcreteASREigenstrain.md) model (or similar model) is used to simulate ASR, then the `expansion_type` for the ASR model should be set to `Isotropic`.
In order for the damage to be applied, the stress needs to be computed using a [ComputeDamageStress](ComputeDamageStress.md) stress calculator.

## Example Input Syntax

!listing test/tests/concrete_expansion_microcracking/concrete_expansion_microcracking.i block=Materials/microcracking

!syntax parameters /Materials/ConcreteExpansionMicrocrackingDamage

!syntax inputs /Materials/ConcreteExpansionMicrocrackingDamage

!syntax children /Materials/ConcreteExpansionMicrocrackingDamage

!bibtex bibliography
