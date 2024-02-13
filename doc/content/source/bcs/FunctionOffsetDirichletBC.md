# FunctionOffsetDirichletBC

!syntax description /BCs/FunctionOffsetDirichletBC

## Description

This boundary condition provides the ability to prescribe the value based on a user specified function evaluated at locations that are offset from the actual node locations by a user specified distance along the nodal normal direction. This boundary condition should be used in conjuction with the `[NodalNormals]`, which supplies the nodal normal direction to the boundary condtion object. This boundary condition can be helpful in transferring solution fields between two different mesh discretizations of a curved domain through `[SolutionUserObject]` in cases when the boundary nodes in the model where boundary conditions are applied are sometimes located slightly outside the domain of the solution mesh.

## Example Input Syntax

!listing test/tests/function_offset_bc/rectangular.i block=BCs/load

`FunctionOffsetDirichletBC` uses `[NodalNormals]` to supply the nodal normal dircetions as shown:

!listing test/tests/function_offset_bc/rectangular.i block=NodalNormals/

!syntax parameters /BCs/FunctionOffsetDirichletBC

!syntax inputs /BCs/FunctionOffsetDirichletBC

!syntax children /BCs/FunctionOffsetDirichletBC

!bibtex bibliography
