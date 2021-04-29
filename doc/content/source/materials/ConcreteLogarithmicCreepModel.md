# ConcreteLogarithmicCreepModel

!syntax description /Materials/ConcreteLogarithmicCreepModel

## Description

This material represents a logarithmic viscoelastic behavior for concrete and other cementitious materials.

The strain is decomposed into three components: an elastic strain $\epsilon_e$, a recoverable viscoelastic strain $\epsilon_r$ and an irrecoverable creep strain $\epsilon_c$. This corresponds to a Burgers type of material with an elastic spring, a Kelvin-Voigt module, and a dashpot placed in series.

The implementation uses the linear viscoelasticity framework defined in MOOSE tensor_mechanics module.

## Elastic Strain

The elastic strain is directly related to the stress $\sigma$ with the usual Hooke's law, where $C_e$ is the fourth-order elasticity tensor of the material. In the current model, $C_e$ is calculated using the `youngs_modulus` and `poissons_ratio` parameters provided by the user.
\begin{equation}
\sigma = C_e : \epsilon_e
\end{equation}

## Irrecoverable Creep Strain

This strain corresponds to the long-term visco-elastic strain of the material. It is linear with the logarithm of time, and cannot be recovered upon unloading. It is calculated using an aging dashpot:
\begin{equation}
\sigma = \left( 1 + \frac{t}{\tau_c} \right) \eta_c C_e : \dot{\epsilon}_c
\end{equation}
$C_e$ is the same elasticity tensor used for the calculation of the elastic strain.

$\eta_c$ is defined using the `long_term_viscosity` parameter, and controls the slope of the creep curve in the logarithmic space.

$\tau_c$ is defined using the `long_term_characteristic_time` parameter (default value of 1 day) and controls when the logarithmic behavior starts. 


## Recoverable Creep Strain

This strain corresponds to the short-term visco-elastic strain of the material, which can be partially recovered upon unloading. It is calculated using a single Kelvin-Voigt module, where $C_r$ is the fourth-order elasticity tensor of the spring and $\eta_r$ the characteristic time of the dashpot.
\begin{equation}
\sigma = C_r : \left( \epsilon_r + \eta_r \dot{\epsilon}_r  \right)
\end{equation}

$C_r$ is calculated using the `recoverable_youngs_modulus` and `recoverable_poissons_ratio` parameters provided by the user, or using `youngs_modulus` and `poissons_ratio` parameters if they are undefined.

$\eta_r$ is defined using the `recoverable_viscosity` parameter, or with the `long_term_characteristic_time` parameter if it is undefined.

To disable the recoverable strain, the user must set the `use_recovery` parameter to FALSE.


## Creep At Elevated Temperature

The model can include the effect on temperature on creep.

The creep strain rates are increased using an Arrhenius-type law, with the `activation_temperature` parameter controlling the increase of strain rate with temperature.

The model assumes that `temperature` and `reference_temperature` are in Celsius, while `activation_temperature` is in Kelvin.


## Creep Under Low Relative Humidity

The model can include the effect of a constant low relative humidity on creep.

In this case, the irrecoverable creep strain rate is decreased linearly with the current relative humidity.

This effect is automatically activated when the user defines a humidity coupled variable.


## Drying Creep

The model can include the effect of drying on the concrete creep, also known as the Pickett effect.

In this case, the characteristic time of the irrecoverable creep strain is modified to account for the increase of the irrecoverable creep strain rate on drying. The new characteristic time depends on $\eta_d / |\dot{h}|$, with $\eta_d$ the `drying_creep_viscosity` parameter provided by the user, and $|\dot{h}|$ the rate of drying.

This effect if accounted in the simulation when the user defines a humidity coupled variable, and $\eta_d$.

!!!
    The calculation of the drying creep requires either `humidity` to be computed on `timestep_begin` (for example using an AuxKernel or a MultiApp transfer), or the `force_recompute_properties` parameter to be set to `TRUE`.


## Time Integration

The visco-elastic strains are integrated in time using a time-stepping procedure defined for linear viscoelastic materials in the tensor_mechanics module.

For the time-stepping scheme to be properly updated, a [LinearViscoelasticityManager](/LinearViscoelasticityManager.md) object must be included in the input file, and linked to the ConcreteLogarithmicCreepModel material

The ConcreteLogarithmicCreepModel is compatible with either the total small strain approximation, or either of the incremental strain approximation (incremental small strains or finite strains). It needs the following stress calculators:

| Strain | Stress | Additional Materials |
| - | - | - |
| ComputeSmallStrain | ComputeLinearViscoelasticStress | none |
| ComputeIncrementalSmallStrain | ComputeMultipleInelasticStress | LinearViscoelasticStressUpdate |
| ComputeFiniteStrain | ComputeMultipleInelasticStress | LinearViscoelasticStressUpdate |

The stress calculators use the actual elasticity tensor of the material $C_r$, which is provided by the ConcreteLogarithmicCreepModel itself.

!syntax parameters /Materials/ConcreteLogarithmicCreepModel

!syntax inputs /Materials/ConcreteLogarithmicCreepModel

!syntax children /Materials/ConcreteLogarithmicCreepModel
