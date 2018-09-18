# Concrete Expansion Function Eigenstrain

!syntax description /Materials/ConcreteExpansionFunctionEigenstrain

The `ConcreteExpansionFunctionEigenstrain` imposes an eigenstrain in which the total volumetric
strain is computed with a MOOSE [`Function`](/Functions/index.md) object defined by the `function`
input parameter. The `prefix` parameter is used to distinguish the `volumetric_strain` variable
defined by other models such as a [`ConcreteASREigenstrain`](/Materials/ConcreteASREigenstrain.md)
or other `ConcreteExpansionFunctionEigenstrain` models.

The actual eigenstrain can either be `Isotropic` or `Anisotropic` depending on the `expansion_type`
parameter. In the latter case, the eigenstrains are redistributed when the material is subject
to structural loads, following the model developped by [cite:saouma_constitutive_2006] for
Alkali Silica Reaction. See the documentation for the [`ConcreteASREigenstrain`](/Materials/ConcreteASREigenstrain.md)
model for more details.

!syntax parameters /Materials/ConcreteExpansionFunctionEigenstrain

!syntax inputs /Materials/ConcreteExpansionFunctionEigenstrain

!syntax children /Materials/ConcreteExpansionFunctionEigenstrain

!bibtex bibliography
