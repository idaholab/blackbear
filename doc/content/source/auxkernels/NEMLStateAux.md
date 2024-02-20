# NEMLStateAux

!syntax description /AuxKernels/NEMLStateAux

This object copies part of the internal state of a NEML material model into an 
`AuxVariable`.  Most often this is used to output the internal state of a
NEML model to an Exodus II output file for visualization.

For non-scalar material properties the user should append an `_i` to the end of the
state variable name in NEML to get the `ith` component of the internal representation of
that object.  For example, symmetric tensor material properties like back stresses have
six components, from `_0` to `_5`, as they are stored in NEML as a Mandel notation vector.

!syntax parameters /AuxKernels/NEMLStateAux

!syntax inputs /AuxKernels/NEMLStateAux

!syntax children /AuxKernels/NEMLStateAux

