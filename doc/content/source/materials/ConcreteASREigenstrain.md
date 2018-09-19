# Concrete ASR Eigenstrain

!syntax description /Materials/ConcreteASREigenstrain

## Alkali-silica reaction (ASR) swelling model

## Background

Deterioration of concrete structures affected by ASR can be attributed, on the microscopic level, to the formation of a hydrophilic gel due to complex dissolution-precipitation
reactions between reactive silica in aggregates, alkaline ions such as potassium $K^+$ and $Na^+$ and hydroxyl ions $OH^-$ in the cement pore solution.
If water is available in concrete pores, the gel swells, creating an
internal pressure in localized regions within concrete structures, which can initiate micro- and macrocracking, excessive expansion, misalignment of
the structure etc. According to [cite:ulm2000thermo], there are two important factors that control the ASR reaction rate:

- ASR reactions are thermally activated. The higher the temperature, the faster they occur. This kinetic effect of temperature on ASR results from the thermoactivation of both the dissolution of reactive silica on aggregate-cement interface and the precipitation of gel.
- The relative humidity in concrete has a strong influence on ASR, affecting both the kinetics and magnitude of volumetric swelling. Water plays an important role as a solvent for silica dissolution, intervenes as a transport medium for diffusion of ions through the pore network, and is a necessary compound for the formation of various reaction products (gels and other mineral precipitates).

The `ConcreteASREigenstrain` model computes the evolution of the ASR reaction and the resulting eigenstrains as a function of the thermal, moisture, and mechanical stresses at local material points, which enables ASR to be simulated in a way that accounts for full coupling of all physics involved in this phenomenon.  It is an implementation of the [cite:saouma_constitutive_2006] ASR swelling model, which builds on the model of [cite:ulm2000thermo]. This model has the following features:

- The ASR reaction rate is temperature dependent
- The ASR reaction can be retarded by compressive stress within concrete structures
- Both high compressive and tensile stress states inhibit ASR expansion due to the formation of micro- and macro-cracks that adsorb the expanding gel
- Triaxial compressive stress states reduce expansion
- The ASR expansion is anisotropic to account for the fact that it is constrained by compression, and is redirected into other less-constrained principal directions

Details of this model's representation of ASR reaction kinetics, volumetric strain, and anisotropic distribution of that strain are provided below. Note that throughout this discussion, the notations of [cite:ulm2000thermo] and [cite:saouma_constitutive_2006] are adopted.

## ASR reaction kinetics

This model employs a 1st-order ASR reaction kinetics model that is dependent on both the temperature and the first invariant of the stress tensor as:
\begin{equation}
    \label{eqn:asr_kinetics}
    t_C(\theta, \xi)\cdot\frac{d\xi}{dt} = \tau_C(\theta)\cdot\frac{1+\exp[-\tau_L(\theta, I_\sigma, f_c')/\tau_C(\theta)]}{\xi +
    \exp[-\tau_L(\theta, I_\sigma, f_c')/\tau_C(\theta)]}\cdot\frac{d\xi}{dt} = 1 - \xi,
\end{equation}
in which $\xi$ is the ASR reaction extent ranging from 0 (not reacted) to 1 (fully reacted), $\theta$ is the temperature, and $\tau_C(\theta)$ and
$\tau_L(\theta, I_\sigma, f_c')$ are expressed as:
\begin{equation}
\begin{aligned}
    \label{eq:theta_I_sigma}
    \tau_C(\theta) = \tau_C(\theta_0)\exp[U_C(1/\theta - 1/\theta_0)] \\
    \tau_L(\theta, I_\sigma, f_c') = f(I_\sigma, f_c')\tau_L(\theta_0)\exp[U_L(1/\theta - 1/\theta_0)]
\end{aligned}
\end{equation}
representing the latency and characteristic times of ASR reactions, respectively. Here $\theta_0$ is the reference temperature (i.e., the temperature at which
stress-free ASR experiments are carried out), $I_\sigma$ is the first invariant of the stress tensor, $f_c'$ is the uniaxial compressive strength of concrete, and
$U_L$ and $U_C$ are thermal activation energy constants for the latency and characteristic times, respectively, and are determined from tests conducted by [cite:larive] as
\begin{equation}
\begin{aligned}
    U_L &= 9400 \pm 500K \\
    U_C &= 5400 \pm 500K
\end{aligned}
\end{equation}

The function $f(I_\sigma, f_c')$ in [eq:theta_I_sigma] represents the effect of compressive stress on the ASR reaction kinetics by modifying the latency time $\tau_L$ and is defined as
\begin{equation}
\begin{gathered}
    \label{eqn:asr_comp_stress_kin}
    f(I_\sigma, f_c') = \left\{\begin{array}{lll}
            1 & if & I_\sigma > 0\\
            1 + \alpha\frac{I_\sigma}{3f_c'} & if & I_\sigma \le 0
        \end{array}\right.
\end{gathered}
\end{equation}
where $I_\sigma = \sigma_{I} + \sigma_{II} + \sigma_{III}$.

[cite:saouma_constitutive_2006] assumed a value of 4/3 for the constant $\alpha$ based on their analysis of experimental data from [cite:multon2006effect]. When $I_\sigma$ is
greater than zero (i.e., tensile stress state), $f(I_\sigma, f_c')$ is equal to 1, indicating no effect of tensile stress on the reaction kinetics. When $I_\sigma$ is
less than zero (i.e., compressive stress state), $f(I_\sigma, f_c')$ has a value greater than 1, indicating an increase in the latency time $\tau_L$ (equivalently
retardation of ASR reaction).

[eqn:asr_kinetics] for the ASR reaction is a nonlinear ordinary differential equation (ODE) that is solved locally at the quadrature points within each finite element by a Newton-Raphson iteration scheme, given the current temperature and the stress tensor and reaction extent at the end of the previous time step.

## Stress-dependent ASR volumetric strain

The volumetric strain resulting from the ASR reaction depends on stress in this model. Once the increment of ASR reaction extent $\Delta\xi$ is obtained, the ASR volumetric strain increment $\Delta\varepsilon_{vol}^{ASR}$ from time $t$ to $t+\Delta t$
is evaluated using the following formula
\begin{equation}
    \label{eqn:asr_volstrn}
    \Delta\varepsilon_{vol}^{ASR} = \Gamma_t(f_t', \sigma_{I}|COD)\Gamma_c(\bar{\sigma}, f_c')g(H)\Delta\xi\varepsilon^{\infty}|_{\theta = \theta_0},
\end{equation}
where $f_t'$ is the tensile strength of the concrete; $\sigma_{I}$ is the maximum principal stress ( $>0$ under tensile stress);
$COD$ is the crack opening displacement; $\bar{\sigma}$ is the ratio between the hydrostatic stress and compressive strength of concrete, and
 $\varepsilon^{\infty}$ is the laboratory-determined maximum free volumetric expansion at the reference temperature $\theta_0$.

The function $g(H)$ in [eqn:asr_volstrn] accounts for the dependency of gel expansion on the water in concrete and takes the form:
\begin{equation}
    g(H) = H^m,
\end{equation}
where the exponent $m$ is an empirical constant and $H$ is the relative humidity in concrete.
$g(H)$ has a value between 0 and 1. 

Function $\Gamma_t(f_t', \sigma_{I}|COD)$ in [eqn:asr_volstrn] accounts for the reduction of ASR expansion due to tensile cracking
(e.g., gels are adsorbed into tensile macrocracks), and takes the following form:
\begin{equation}
    \Gamma_t = \left\{\begin{array}{lll}
                    1 & if & (\sigma_{I} \le \gamma_t f_t') \\
                    \Gamma_r + (1 - \Gamma_r)\frac{\gamma_t f_t'}{\sigma_{I}} & if & (\gamma_t f_t' \le \sigma_{I})
                \end{array}\right.
\end{equation}
in which $\gamma_t$ is the fraction of the tensile strength beyond which gel is adsorbed by cracks and is a user input parameter in simulation, and
$\Gamma_r$ is a residual ASR expansion retention factor for ASR under tension.

Function $\Gamma_c(\bar{\sigma}, f_c')$ in [eqn:asr_volstrn] accounts for the reduction in ASR volumetric expansion under compressive stress state, in which gel is absorbed by
diffused microcracks, and takes the following form:
\begin{equation}
    \label{eqn:gammac}
    \Gamma_c = \left\{\begin{array}{llll}
            1 & if & \bar{\sigma} \le 0 & \text{tension} \\
            1 - \frac{e^\beta\bar{\sigma}}{1 + (e^\beta - 1)\bar{\sigma}} & if & \bar{\sigma} > 0 & \text{compression}
        \end{array}\right.
\end{equation}
where the exponent $\beta$ is an empirical constant between -2 and 2, and $\bar{\sigma}$ is the ratio between the hydrostatic stress and compressive strength of concrete:
\begin{equation}
    \bar{\sigma} = \frac{\sigma_{I} + \sigma_{II} + \sigma_{III}}{3f_c'}
\end{equation}

## Anisotropic ASR strain distribution

In general the incremental ASR volumetric stain $\Delta\varepsilon_{vol}^{ASR}$ should not be applied isotropically because the propensity for expansion in a given direction depends on the stress in that direction. The technique of [cite:saouma_constitutive_2006] is used here to calculate the distribution of volumetric expansion among the three directions of principal stress based on the behavior under either uniaxial, biaxial or triaxial confinement conditions.

Given the full stress tensor (in Cartesian coordinates) at a quadrature point within an element, an eigensolver is used to obtain the three principal stresses,
$\sigma_k$, $\sigma_l$ and $\sigma_m$, and associated eigenvectors for the directions of principal stresses, $R_k$, $R_l$ and $R_m$.
These eigenvectors form a stress/strain rotational matrix $R=(R_k,R_l,R_m)$ that is used later to rotate the incremental ASR strain tensor expressed in principal stress/strain coordinates back into Cartesian coordinates.

!media media/weight_regions.png
      id=asr_interp
      style=width:50%;
      caption=2D stress-space map and ASR weight interpolation quadrants (based on [cite:saouma_constitutive_2006])

The weight allocation scheme for the principal directions starts by dividing the stress space into nine quadrants using the concrete tensile strength $f_t$ and
compressive strength $f_c'$ as well as a gel expansion-inhibiting compressive stress $\sigma_u$, as shown in [asr_interp].  $\sigma_u$ is defined as the compressive stress
beyond which no further gel expansion can occur. [cite:saouma_constitutive_2006] proposed a value of -10MPa based on previous
experimental studies [cite:larive]. Any combination of two principal stresses will fall into one of the nine quadrants of this two-dimensional stress map,
which has a total of 16 nodes.

To calculate the ASR expansion weight along the direction of a particular principal stress, using $\sigma_k$ for example, the following three steps are taken:

- Identify the quadrant encompassing the other two principal stresses $\sigma_l$ and $\sigma_m$ using [asr_interp] and the corresponding nodal numbers of that quadrant;
- Find the rows of [triax_weights] corresponding to the identified nodal numbers and determine the nodal weights $W_i(\sigma_k), i=1, 2, 3, 4$ from the last 3 columns of [triax_weights] in the corresponding rows using a linear interpolation of $\sigma_k$.
- Finally, compute the ASR expansion weight $W_k(\sigma_k, \sigma_l, \sigma_m)$ for the principal direction $k$ using the four nodal weights of the quadrant in which $(\sigma_l, \sigma_m)$ falls using the bilinear interpolation of [eqn:wk]

The bilinear interpolation is performed in the same way that solution fields are interpolated in the standard finite element method:
\begin{equation}
    \label{eqn:wk}
    W_k(\sigma_k, \sigma_l, \sigma_m) = \Sigma_{i=1}^4N_i(\sigma_l, \sigma_m)W_i(\sigma_k),
\end{equation}
where $N_i$ is the bilinear shape function similar to those used in finite element method and is given by
\begin{equation}
\begin{gathered}
    N(\sigma_l, \sigma_m) = \frac{1}{ab} \lfloor \begin{array}{llll}
    (a-\sigma_l)(b-\sigma_m), & \sigma_l(b-\sigma_m), &  \sigma_l\sigma_m, & (a - \sigma_l)\sigma_m \end{array} \rfloor \\
    W(\sigma_k) = \lfloor \begin{array}{llll}
        W_1(\sigma_k), & W_2(\sigma_k), & W_3(\sigma_k), & W_4(\sigma_k)
    \end{array}\rfloor^T \\
    \begin{array}{ll}
        a = (a_1|a_2|a_3) & b = (b_1|b_2|b_3)
    \end{array} \\
    \begin{array}{ll}
        \sigma_l = (\sigma_l|\sigma_l - \sigma_u) & \sigma_m = (\sigma_m|\sigma_m - \sigma_u)
    \end{array}
\end{gathered}
\end{equation}

!table id=triax_weights
       caption=Triaxial weights for nodes in the stress map of [asr_interp] (from [cite:saouma_constitutive_2006])
      style=width:70%;
| Node Number | $\sigma_l$ | $\sigma_m$ | $\sigma_k \le 0$ | $\sigma_k = \sigma_u$ | $\sigma_k = f_c'$ |
|-------------|------------|------------|------------------|-----------------------|-------------------|
|       1     |      $0$   |     $0$    |         $1/3$    |   $0$                 |       $0$         |
|       2     | $\sigma_u$ |        $0$ |         $1/2$    |   $0$                 |       $0$         |
|       3     | $\sigma_u$ | $\sigma_u$ |           $1$    | $1/3$                 |       $0$         |
|       4     |        $0$ | $\sigma_u$ |         $1/2$    |   $0$                 |       $0$         |
|       5     |      $f_c$ |        $0$ |         $1/2$    |   $0$                 |       $0$         |
|       6     |      $f_c$ | $\sigma_u$ |           $1$    | $1/2$                 |       $0$         |
|       7     |      $f_c$ |      $f_c$ |           $1$    |   $1$                 |     $1/3$         |
|       8     | $\sigma_u$ |      $f_c$ |           $1$    | $1/2$                 |       $0$         |
|       9     |        $0$ |      $f_c$ |         $1/2$    |   $0$                 |       $0$         |
|      10     |      $f_t$ |      $f_c$ |         $1/2$    |   $0$                 |       $0$         |
|      11     |      $f_t$ | $\sigma_u$ |         $1/2$    |   $0$                 |       $0$         |
|      12     |      $f_t$ |        $0$ |         $1/3$    |   $0$                 |       $0$         |
|      13     |      $f_t$ |      $f_t$ |         $1/3$    |   $0$                 |       $0$         |
|      14     |        $0$ |      $f_t$ |         $1/3$    |   $0$                 |       $0$         |
|      15     | $\sigma_u$ |      $f_t$ |         $1/2$    |   $0$                 |       $0$         |
|      16     |      $f_c$ |      $f_t$ |         $1/2$    |   $0$                 |       $0$         |


The previous steps are then repeated to calculate the ASR expansion weights along the other two principal directions, $W_l(\sigma_k, \sigma_l, \sigma_m)$ and
 $W_m(\sigma_k, \sigma_l, \sigma_m)$. Note that the summation of $W_k$, $W_l$ and $W_m$ equals 1.
The individual incremental ASR strains along the principal directions are then obtained using these relative weights by the following formula:
\begin{equation}
\label{eqn:asr_principal}
\begin{array}{llll}
    \Delta\varepsilon_{i}^{ASR} = W_i\Delta\varepsilon_V^{ASR}, &i =1, 2&and&3.
\end{array}
\end{equation}

Finally, the full ASR expansion-induced incremental strain tensor $\Delta\varepsilon^{ASR}$ in the Cartesian coordinate system can then be obtained by
rotating $\Delta\varepsilon_{i}^{ASR}$ into the current coordinates via
\begin{equation}
\label{eqn:asr_strain}
\Delta\varepsilon^{ASR}=R\{\Delta\varepsilon_{i}^{ASR}\}R^T.
\end{equation}
in which $\Delta\varepsilon^{ASR}$ is the imposed incremental strain tensor due to ASR expansion at each quadrature point, which drives the deformation of the concrete structure.

## ASR-induced Damage

The ASR-induced degradation of the mechanical properties of concrete can be simulated with a specific damage model.
In this approach, the stress is computed with a [ComputeDamageStress](ComputeDamageStress.md) calculator as:

\begin{equation}
    \sigma = (1 - d^{ASR}) \mathbb{E} : \left[\varepsilon - \varpesilon^{ASR} \right]
\end{equation}

in which $\mathbb{E}$ is the fourth-order stiffness tensor of the material and $d^{ASR}$ the ASR-induced damage.

Two options are available to compute $d^{ASR}$:

- Using the [ConcreteASRMicrocrackingDamage](ConcreteASRMicrocrackingDamage.md) model based on [cite:saouma_constitutive_2006], in which $d^{ASR}$ is a simple function of the ASR reaction extent $\xi(t,\theta)$ and $\beta_E$ a material constant representing the loss of modulus when the material has fully reacted:
  \begin{equation}
  d^{ASR}(t, \theta) = (1 - \beta_E)\xi(t, \theta)
  \end{equation}
- Using the [ConcreteExpansionMicrocrackingDamage](ConcreteExpansionMicrocrackingDamage.md) model, in which the damage is computed as a function of the total ASR strain and the current stress. In this approach, the anisotropic effects of ASR are indirectly accounted for via the damage model rather than the eigenstrain itself, and therefore this model should be used with the `expansion_type = Isotropic` option.

## Implementation and Usage

The `ConcreteASREigenstrain` model computes the ASR reaction extent $\xi$, and stores it in a property named `ASR_extent`. It also computes an eigenstrain tensor that is used to modify the strain tensor. The loss of mechanical properties due to ASR can be represented with the [ConcreteASRMicrocrackingDamage](ConcreteASRMicrocrackingDamage.md) model or the [ConcreteExpansionMicrocrackingDamage](ConcreteExpansionMicrocrackingDamage.md) model, provided that the stress is computed with [ComputeDamageStress](ComputeDamageStress.md).

!syntax parameters /Materials/ConcreteASREigenstrain

!syntax inputs /Materials/ConcreteASREigenstrain

!syntax children /Materials/ConcreteASREigenstrain

!bibtex bibliography
