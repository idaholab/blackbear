# Steel Creep Damage (Oh et al)

!syntax description /Materials/SteelCreepDamageOh

## Description

The `SteelCreepDamageOh` can model the mechanical failure of steel due to creep (see [!cite](oh2011creep) and [!cite](OH2011124)).

Accumulated damage ($\omega$) is computed by accumulating the creep strain rate normalized with the multi-axial creep ductility. The accumulated damage is 
\begin{equation}
\omega=\sum{\Delta\omega = \sum{\frac{\Delta\varepsilon_{x}}{\varepsilon^{*}_{f}}}}
\end{equation}

The triaxial creep ductility $\varepsilon^{*}_{f}$ can be obtained from the uniaxial creep ductility $\varepsilon_f$ using the following expression

\begin{equation}
\varepsilon^{*}_{f} = \varepsilon_{f} \frac{\sinh{\frac{2.0}{3.0} \frac{n - 0.5}{n + 0.5}}}{\sinh{2 \frac{n - 0.5}{n + 0.5} h}}
\end{equation}

where $n$ is the creep power law exponent, $h$ is the stress triaxiality.

!alert note
The accumulated damage $\omega$, as defined in [!cite](OH2011124), can be requested by the user as an auxiliary variable. In addition, the more traditional `damage_index`, which represents a linear change in the stress value, can also be requested. This class internally casts the value of $\omega$ into the `damage_index`.

## Implementation and Usage

The effects of damage can be applied all at once or progressively. In essence, this object allows the user to select an initial value of the damage index at which the load carrying capacity of the finite element will start to get decreased linearly (`reduction_damage_threshold`). In addition, the user can select the factor that will reduce the load-carrying capacity (stress) through the input parameter `reduction_factor`. These two parameters should be chosen such that material damage consistently reduces material point's stresses. For example, for steel, `reduction_damage_threshold` should be selected to be close to 1.0 so that the load-carrying capacity of the finite element is lost quickly. 

Damaged elements may get distorted and, therefore, may generate increased values of creep strains which may, in turn, limit the maximum allowable material time step. To avoid these undesired effects, the user may combine damage models with [CoupledVarThresholdElementSubdomainModifier](CoupledVarThresholdElementSubdomainModifier.md). The `CoupledVarThresholdElementSubdomainModifier` user object can eliminate or move damage elements from selected mesh blocks. In addition, it is strongly recommended to toggle the `use_old_damage` flag to true for this object to yield optimal convergence properties.

## Example Input Syntax

!listing test/tests/steel_creep_damage_oh/steel_creep_damage.i block=Materials/damage

`SteelCreepDamage` is run in conjunction with a stress calculator that supports the use of a damage model, ([ComputeMultipleInelasticStress](ComputeMultipleInelasticStress.md) in this case), as shown:

!listing test/tests/steel_creep_damage_oh/steel_creep_damage.i block=Materials

!syntax parameters /Materials/SteelCreepDamageOh

!syntax inputs /Materials/SteelCreepDamageOh

!syntax children /Materials/SteelCreepDamageOh

!bibtex bibliography