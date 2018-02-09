<!-- MOOSE Documentation Stub: Remove this when content is added. -->

# ConcreteDryingShrinkage

This material represents the eigenstrains induced by drying in concrete. The shrinkage strain $\epsilon_{sh}$ is calculated as:

$$
\epsilon_{sh} = -k_{sh} (1 - h)^d
$$

Where $k_{sh}$ is the ``drying_shrinkage_coefficient`` (which must be positive), $h$ the relative humidity (a MOOSE variable which value must range between 0 and 1), and $d$ a positive ``exponent`` (typically equal to 1). The drying shrinkage is isotropic and reversible.

It can be set partially irreversible by defining the ``irreversibility_threshold``, in which case the shrinkage strain caused by relative humidity lower than that threshold will be deemed irreversible and not recovered upon rewetting.

## Description

!syntax description /Materials/ConcreteDryingShrinkage

!syntax parameters /Materials/ConcreteDryingShrinkage

!syntax inputs /Materials/ConcreteDryingShrinkage

!syntax children /Materials/ConcreteDryingShrinkage
