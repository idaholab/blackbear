# NEML Thermal Expansion Eigenstrain

!syntax description /Materials/NEMLThermalExpansionEigenstrain

# Thermal Eigenstrain Obtained from NEML

This material model provides an interface to use the [Nuclear Engineering Material model Library (NEML)](https://github.com/Argonne-National-Laboratory/neml) developed by Argonne National Laboratory to provide thermal expansion eigenstrains. This model would typically be used together with the [NEMLStress](NEMLStress.md) model, the documentation of which provides more details on integration with that library.

NEML permits the definition of complex material constitutive models, the details of which are defined in an XML file that is independent of the MOOSE input file. The required inputs for this model on the MOOSE side simply provide the location of the XML file, which contains a database of potentially multiple material models, and the name of the model within that database to be used.  This model computes the eigenstrain induced by thermal expansion.

!syntax parameters /Materials/NEMLThermalExpansionEigenstrain

!syntax inputs /Materials/NEMLThermalExpansionEigenstrain

!syntax children /Materials/NEMLThermalExpansionEigenstrain

!bibtex bibliography
