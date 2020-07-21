# ASR Validation Cases

Alkalli-silica reaction in concrete significantly influences the expansion behavior and overall degradation of the concrete structures. ASR expansion is highly dependent on the components of the concrete such as cement types, aggregate types and sizes etc. In addition, environmental conditions such as temperature and humidity also influence the ASR reaction rate. Hence, it is very difficult to obtain a generic ASR expansion model applicable to different concrete. In BlackBear, the ASR expansion model is implemented based on [!cite](saouma_constitutive_2006) ASR swelling model, which builds on the model of [!cite](ulm2000thermo). In this model, the reaction rate and the maximum expansion varies with the reactivity of the concrete components and environmental conditions. The details about the model can be found in [ConcreteASREigenstrain](ConcreteASREigenstrain.md).

Various researchers have attempted to experimentally observe the ASR behavior in concrete and identify various criteria for ASR expansion. The validations cases in BlackBear are built based on such experiments. These cases not only focuses on the ASR expansion model, but also tests the multi-physics concrete modeling capability of BlackBear.

## Benchmark Problem

[!cite](wallau2018asr) performed prism testing to assess the susceptibility of the concrete structure due to ASR. They demonstrated that the ASR expansion happens in three stages, initial swelling, acceleration, and consolidation of expansion, that is captured by a S-curve. They also showed that the curves could vary widely depending on the aggregate type and concluded that a globally applicable ASR expansion curve might not exist. The initial benchmark cases in Blackbear are built based on these experiments demonstrating how well the model can predict the experimental observations. The properties associated with the ASR extent calculation are tabulated below:

| Model Parameters | Aggregate A | Aggregate B | Aggregate C |
| - | - | - | - |
| Maximum Volumetric Expansion | 1.24 | 0.76 | 0.38 |
| Characteristic Time (days) | 8.68 | 66.84 | 17.24 |
| Latency Time (days) | 16.22 | -126.1 | -2.55 |

Here, the negative latency time indicates the aggregates are highly reactive. Refer to [ConcreteASREigenstrain](ConcreteASREigenstrain.md) and [!cite](wallau2018asr) for more details.

## Assessment Problems

[!cite](wald_2017) performed a series of experiments to understand how the presence of multiaxial reinforcement affects ASR expansion behavior of concrete.
Two of these experiments are included here as assessment cases: one unreinforced block (Specimen A1-000b), and one block with nine uniaxial bars oriented in the z-direction (Specimen A1-001a). The assessment cases also evaluate the effect of the environmental condition on the expansion behavior of the concrete.

## Validation Cases

!listing test/tests/concrete_ASR_validation/tests

!listing assessment/ASR_Validation/wald2017B/assessment


## Input Files

!listing test/tests/concrete_ASR_validation/asr_validation.i

!listing assessment/ASR_Validation/wald2017B/asr_concrete_block_calibration.i

!listing assessment/ASR_Validation/wald2017B/asr_concrete_block_validation_rebar.i


!bibtex bibliography
