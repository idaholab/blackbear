# FunctionOffsetDirichletBC

!syntax description /BCs/FunctionOffsetDirichletBC

## Description

This boundary condition provides the ability to prescribe a user specified function at nodes offset from the boundary nodes by a user specified distance along the nodal normal direction. This boundary condition should be used in conjuction with the `[NodalNormals]`, which supplies the nodal normal direction to the boundary condtion object. This boundary condition can be helpful in transferring solution fields between two different mesh discretizations of a curved domain through `[SolutionUserObject]`.

## Example Input Syntax

!listing test/tests/function_offset_bc/rectangular.i block=BCs/load

`FunctionOffsetDirichletBC` uses `[NodalNormals]` to supply the nodal normal dircetions as shown:

!listing test/tests/function_offset_bc/rectangular.i block=NodalNormals/

!syntax parameters /BCs/FunctionOffsetDirichletBC

!syntax inputs /BCs/FunctionOffsetDirichletBC

!syntax children /BCs/FunctionOffsetDirichletBC

!bibtex bibliography
