# NEML Stress

!syntax description /Materials/NEMLStress

# Stress Calculator Based on NEML Constitutive Models

This material model provides an interface to the constitutive models for structural materials provided by the [Nuclear Engineering Material model Library (NEML)](https://github.com/Argonne-National-Laboratory/neml) developed by Argonne National Laboratory. This is a general interface to all of the models provided by that library, which are documented in detail in the [NEML documentation](https://neml.readthedocs.io/en/stable/).

NEML permits the definition of complex material constitutive models, the details of which are defined in an XML file that is independent of the MOOSE input file. The required inputs for this model on the MOOSE side simply provide the location of the XML file, which contains a database of potentially multiple material models, and the name of the model with in that database to be used.  This model computes the stress, elasticity tensor, and material Jacobian.

Using this model requires that the NEML source code is present, and that the prerequisite libraries are available.
NEML is provided as a submodule in BlackBear under the directory `blackbear/contrib/neml`.
The up-to-date version of the NEML submodule is obtained using:

```
git submodule init
git submodule update
```

in the `blackbear` directory.
NEML is compiled using the MOOSE build system, so no separate compilation step is required to build the library.
However, NEML has dependencies on the BOOST library that are not present in the default MOOSE environment.
To build with boost, one must simply load the `boost` module provided with MOOSE:

```
module load advanced_modules boost
```

and rebuild libMesh with the `--with-boost` option from the moose directory:

```
cd moose
./scripts/update_and_rebuild_libmesh.sh --with-boost
```

This option instructs libMesh to be compiled with the full BOOST installation provided by the `boost` module, rather than the subset of BOOST that is distrubted with libMesh.  
MOOSE and Blackbear must also be rebuilt.  Rebuilding Blackbear with these changes to include the BOOST library will then automatically build NEML.  

Alternatively, if you are using Conda to manage your MOOSE environment instead of modules, a custom Conda MOOSE environment 
will need to be created that does not include the prebuilt libMesh library. This Conda environment is set-up with the following commands:

```
conda create -n moose-boost
conda activate moose-boost
conda install moose-petsc moose-tools boost
```

Once the above Conda environment is set-up, the custom libMesh library containing boost is built using the script and options previously described above.


## Example Input Syntax

!listing test/tests/neml_simple/le_stress.i block=Materials/stress

The standard usage of `NEMLStress` is shown in this input file, where all material properties are provided in the xml file as shown:

!listing test/tests/neml_complex/examples.xml start=<materials end=<variable_example

A NEML xml file starts with the xml root `<materials>` and with in this root, multiple material models are defined within separate xml elements.  
This example xml file contains three material models, i.e. simple_elastic, variable_example, complex_example.  
The `model` keyword is used to select a particular model from the NEML xml file.  The model name in the xml file is 
given in the xml tag that that begin and end the xml element.  The xml tag containing the material model also contains the model type. 

## Example Input Syntax with Parameter Substitution

!listing test/tests/neml_simple/le_stress_variableOverwrite.i block=Materials/stress

Alternatively, this input file uses optional parameters that allow some of the NEML 
model parameters that would ordinarily be defined in the xml file to be defined 
in the MOOSE input file.  This option is provided to allow selected values to be varied for sampling using the `stochastic_tools` module.
The numerical values that are ordinarily used in the NEML xml file must be replaced 
by unique string identifiers enclosed in `{}`, which are then referenced in the 
`neml_variable_iname` parameter in the MOOSE input file.  
In this example, these identifiers are `PoissonsVar` and `YoungsVar`. 

!listing test/tests/neml_complex/examples.xml start=<variable_example end=<complex_example

The numerical values that get substituted for the identifiers defined in `neml_variable_iname` are prescribed
using the `neml_variable_value0` through `neml_variable_value7` parameters.  
The numbers at the end of these parameters indicate the position in the `neml_variable_iname` vector of the identifier to be substituted.
Up to 8 parameters are allowed to be defined in this way.
These are specifed using separate parameters, rather than in a vector, to allow for their values to 
be sampled using `stochastic_tools` which does not currently support modifying parameter vector entries.

!syntax parameters /Materials/NEMLStress

!syntax inputs /Materials/NEMLStress

!syntax children /Materials/NEMLStress

!bibtex bibliography
