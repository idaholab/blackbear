# Mazars Damage

!syntax description /Materials/MazarsDamage

## Description

The `MazarsDamage` model is an implementation of the isotropic damage model for concrete described in [cite:mazars_application_1984] and [cite:pijaudier-cabot_damage_2001].  It is based on the theory of elasticity coupled with the isotropic damage. It describes the elastic stiffness degradation and the softening behavior of concrete under both uniaxial tension and uniaxial compression using an isotropic scalar damage variable $d$. The damage depends only on the effective stresses applied to an undamaged area.

The evolution of the damage is determined by an equivalent strain $\tilde{\varepsilon}$ that quantifies the amount of the local extension state during the mechanical loading. It is calculated from the positive eigenvalues of the strain tensor as follows:
\begin{equation}
\tilde{\varepsilon}=\sqrt{\sum_{i=1}^3\bigl(\langle\varepsilon_i\rangle_+\bigr)^2}
\end{equation}
where $\varepsilon_i$ are the components of principal strain and $\langle.\rangle_+$ is the Macualay bracket, defined as
\begin{equation}
\langle\varepsilon_i\rangle_+ = \Bigg\{
\begin{array}{cc}
\varepsilon_i & \varepsilon_i>0\\
0 & \varepsilon_i<=0
\end{array}
\end{equation}

The damage loading function is:
\begin{equation}
f(\tilde{\varepsilon},\kappa)=\tilde{\varepsilon}-\kappa
\end{equation}
where $\kappa$ is the parameter defining the threshold of damage growth.  Initially, $\kappa$ is equal to $f_t/E_0$, where $f_t$ is the tensile strength and $E_0$ is the initial Young's modulus of the material.  During subsequent steps, it takes on the maximum value of the equivalent strain during the entire load history:
\begin{equation}
\kappa = \max(\tilde{\varepsilon},\kappa_0)
\end{equation}

To account for the differing behavior of the concrete in tension and in compression, the scalar damage $d$ is expressed as a combination of two damage modes:
\begin{equation}
d=\alpha_td_t + \alpha_cd_c
\end{equation}
where $\alpha_t$ and $\alpha_c$ are dimensionless coefficients and represent the contribution of each loading mode such that $\alpha_t=1$ and $\alpha_c=0$ under uniaxial tension, and $\alpha_t=0$ and $\alpha_c=1$ under uniaxial compression. These coefficients are defined as functions of the principal components of the strain tensors $\varepsilon_{ij}^t$ and $\varepsilon_{ij}^c$ that result from the tensile and compressive components of the stress tensor, respectively, defined as: 
\begin{equation}
\varepsilon_{kl}^t=(1-d)C_{ijkl}^{-1}\sigma_{ij}^t
\end{equation}
\begin{equation}
\varepsilon_{kl}^c=(1-d)C_{ijkl}^{-1}\sigma_{ij}^c
\end{equation}
$\alpha_t$ and $\alpha_c$ are then computed as:
\begin{equation}
\alpha_t=\sum_{i=1}^3\biggl(\frac{\langle\varepsilon_i^t\rangle\langle\varepsilon_i\rangle}{\tilde{\varepsilon}^2}\biggr)^\beta
\end{equation}
\begin{equation}
\alpha_c=\sum_{i=1}^3\biggl(\frac{\langle\varepsilon_i^c\rangle\langle\varepsilon_i\rangle}{\tilde{\varepsilon}^2}\biggr)^\beta
\end{equation}
The exponent $\beta$ in these equations is a parameter that can be tuned to fit experimental data for shear response, and is recommended initially to be set to 1. The current implementation of the model has this parameter hard-coded to 1, and it cannot be set by the user.

The scalar damage variables $d_t$ and $d_c$ for tension and compression, respectively, are computed as:
\begin{equation}
d_t=1 - \frac{\kappa_0(1-A_t)}{\kappa} - \frac{A_t}{\exp[B_t(\kappa-\kappa_0)]}
\end{equation}
\begin{equation}
d_c=1 - \frac{\kappa_0(1-A_c)}{\kappa} - \frac{A_c}{\exp[B_c(\kappa-\kappa_0)]}
\end{equation}
where $A_t$, $A_c$, $B_t$, and $B_c$ are material parameters that control the shape of the nonlinear response and are determined using compression and tension tests.

## Implementation and Usage

Damage models such as this do not directly compute the stress, but are used in conjunction with a stress calculator to modify the stress computation that takes other phenomena into account as well. The damage models derive from a common base class that defines functionality that a damage model must supply. The [ComputeDamageStress](ComputeDamageStress.md) model is a stress calculating model that computes the behavior of materials that only exhibit damaged isotropic elastic behavior. The example below shows how the damage and stress models are used in conjunction with each other.

## Example Input Syntax

!listing test/tests/mazars_damage/mazars.i block=Materials/damage

`MazarsDamage` is run in conjunction with a stress calculator that supports the use of a damage model, ([ComputeDamageStress](ComputeDamageStress.md) in this case), as shown:

!listing test/tests/mazars_damage/mazars.i block=Materials/stress

!syntax parameters /Materials/MazarsDamage

!syntax inputs /Materials/MazarsDamage

!syntax children /Materials/MazarsDamage

!bibtex bibliography
