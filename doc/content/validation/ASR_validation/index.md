# ASR Validation Cases

Alkalli-silica reaction in concrete significantly influences the expansion behavior and overall degradation of the concrete structures. ASR expansion is highly dependent on the components of the concrete such as cement types, aggregate types and sizes etc. In addition, environmental conditions such as temperature and humidity also influence the ASR reaction rate. Hence, it is very difficult to obtain a generic ASR expansion model applicable to different concrete. In BlackBear, the ASR expansion model is implemented based on [!cite](saouma_constitutive_2006) ASR swelling model, which builds on the model of [!cite](ulm2000thermo). In this model, the reaction rate and the maximum expansion varies with the reactivity of the concrete components and environmental conditions. The details about the model can be found in [ConcreteASREigenstrain](ConcreteASREigenstrain.md).

Various researchers have attempted to experimentally observe the ASR behavior in concrete and identify various criteria for ASR expansion. The validations cases in BlackBear are built based on such experiments. These cases not only focuses on the ASR expansion model, but also tests the multi-physics concrete modeling capability of BlackBear.

[!cite](wallau2018asr) performed prism testing to assess the susceptibility of the concrete structure due to ASR. They demonstrated that the ASR expansion happens in three stages, initial swelling, acceleration, and consolidation of expansion, that is captured by a S-curve. They also showed that the curves could vary widely depending on the aggregate type and concluded that a globally applicable ASR expansion curve might not exist. The initial benchmark cases in Blackbear are built based on these experiments demonstrating how well the model can predict the experimental observations. The properties associated with the ASR extent calculation are tabulated below:

| Model Parameters | Aggregate A | Aggregate B | Aggregate C |
| - | - | - | - |
| Maximum Volumetric Expansion | 1.24 | 0.76 | 0.38 |
| Characteristic Time (days) | 8.68 | 66.84 | 17.24 |
| Latency Time (days) | 16.22 | -126.1 | -2.55 |

Here, the negative latency time indicates the aggregates are highly reactive. Refer to [ConcreteASREigenstrain](ConcreteASREigenstrain.md) and [!cite](wallau2018asr) for more details.

## Validation Cases


!listing test/tests/concrete_ASR_validation/tests

## Input File


!listing test/tests/concrete_ASR_validation/asr_validation.i


!bibtex bibliography
