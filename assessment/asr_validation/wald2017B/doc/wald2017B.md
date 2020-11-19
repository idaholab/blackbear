# ASR Validation Cases

Alkalli-silica reaction in concrete significantly influences the expansion behavior and overall degradation of the concrete structures. ASR expansion is highly dependent on the components of the concrete such as cement types, aggregate types and sizes etc. In addition, environmental conditions such as temperature and humidity also influence the ASR reaction rate. Hence, it is very difficult to obtain a generic ASR expansion model applicable to different concrete. In BlackBear, the ASR expansion model is implemented based on [!cite](saouma_constitutive_2006) ASR swelling model, which builds on the model of [!cite](ulm2000thermo). In this model, the reaction rate and the maximum expansion varies with the reactivity of the concrete components and environmental conditions. The details about the model can be found in [ConcreteASREigenstrain](ConcreteASREigenstrain.md).

Various researchers have attempted to experimentally observe the ASR behavior in concrete and identify various criteria for ASR expansion. The validations cases in BlackBear are built based on such experiments. These cases not only focuses on the ASR expansion model, but also tests the multi-physics concrete modeling capability of BlackBear.

## Free Expansion of Plain Concrete

[!cite](wallau2018asr) performed prism testing to assess the susceptibility of the concrete structure due to ASR. They demonstrated that the ASR expansion happens in three stages, initial swelling, acceleration, and consolidation of expansion, that is captured by a S-curve. They also showed that the curves could vary widely depending on the aggregate type and concluded that a globally applicable ASR expansion curve might not exist. The initial benchmark cases in Blackbear are built based on these experiments demonstrating how well the model can predict the experimental observations. The properties associated with the ASR extent calculation are tabulated below:

| Model Parameters | Aggregate A | Aggregate B | Aggregate C |
| - | - | - | - |
| Maximum Volumetric Expansion | 1.24 | 0.76 | 0.38 |
| Characteristic Time (days) | 8.68 | 66.84 | 17.24 |
| Latency Time (days) | 16.22 | -126.1 | -2.55 |

Here, the negative latency time indicates the aggregates are highly reactive. Refer to [ConcreteASREigenstrain](ConcreteASREigenstrain.md) and [!cite](wallau2018asr) for more details.

## Free Expansion of Reinforced Concrete Blocks

[!cite](wald_2017) performed a series of experiments to understand how the presence of multiaxial reinforcement affects ASR expansion behavior of concrete.
Two of these experiments are included here as assessment cases: one unreinforced block (Specimen A1-000b), and one block with nine uniaxial bars oriented in the z-direction (Specimen A1-001a). The assessment cases also evaluate the effect of the environmental condition on the expansion behavior of the concrete.

## Validation Cases

!listing test/tests/concrete_ASR_validation/tests

!listing wald2017B/analysis/assessment


## Input Files

!listing test/tests/concrete_ASR_validation/asr_validation.i

!listing wald2017B/analysis/asr_concrete_block_calibration.i

!listing wald2017B/analysis/asr_concrete_block_validation_rebar.i

## Plots

!media figures/temp_rh_history3.png
  id=temp_rh_history3
  caption=Time history of temperature (top) and relative humidity (bottom), including reported values from experiments [!citep](wald_2017) applied to the boundary of the BlackBear model, along with average values computed in the plain and reinforced blocks in the BlackBear models.


!media figures/conc_calibration3.png
  id=conc_calibration3
  caption=Experimental and simulatino results for axial expansion of an unrestrained concrete block (Specimen A1-00b) used for model calibration.


!media figures/plain_conc_sets3.png
  id=plain_conc_sets3
  caption=BlackBear simulation results showing the effect of the environmental conditions on the free expansion of a plain concrete block. The start time of exposure fo the block to the temperature and relative humidity hisotry shown in [temp_rh_history3] is varied to match the times used for the three sets of experiments conducted by [!citep](wald_2017). Here, Set-1 corresponds to Apecimen A1-000b of [!citep](wald_2017), but plain concrete speciments were not actually tested in experimental Sets 2 and 3.


!media figures/uniaxial_rebar3.png
  id=uniaxial_rebar3
  caption=Experimental and simulation results for acial expansion of a concrete block with uniaxial reinforcement (Specimen A1-001a), showing the effect of reinforcement on the reponse in the three directons. The expansion in the x- and y-directions are nearly identical, and overlay each other.


!bibtex bibliography
