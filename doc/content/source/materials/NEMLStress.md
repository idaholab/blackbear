# NEML Stress

!syntax description /Materials/NEMLStress

# Stress Calculator Based on NEML Constitutive Models

This material model provides an interface to the constitutive models for structural materials provided by the [Nuclear Engineering Material model Library (NEML)](https://github.com/Argonne-National-Laboratory/neml) developed by Argonne National Laboratory. This is a general interface to all of the models provided by that library, which are documented in detail in the [NEML documentation](https://neml.readthedocs.io/en/stable/).

NEML permits the definition of complex material constitutive models, the details of which are defined in an XML file that is independent of the MOOSE input file. The required inputs for this model on the MOOSE side simply provide the location of the XML file, which contains a database of potentially multiple material models, and the name of the model within that database to be used.  This model computes the stress, elasticity tensor, and material Jacobian.

Using this model requires that the NEML source code is present, and that the prerequisite libraries are available. NEML is provided as a submodule in BlackBear under the directory `blackbear/contrib/neml`. The up-to-date version of the NEML submodule is obtained using:
```
git submodule init
git submodule update
```
in the `blackbear` directory.

NEML is compiled using the MOOSE build system, so no separate compilation step is required to build the library. However, NEML has dependencies on the BOOST library that is not present in the default MOOSE environment.

To build with BOOST, one must simply load the `boost` module provided with MOOSE:
```
module load advanced_modules boost
```
and add the `--with-boost` option to the `update_and_rebuild_libmesh.sh` script when compiling libMesh to instruct it to use the full BOOST instalation provided by the `boost` module, rather than the subset of BOOST that is distributed with libmesh.

!syntax parameters /Materials/NEMLStress

!syntax inputs /Materials/NEMLStress

!syntax children /Materials/NEMLStress

!bibtex bibliography
