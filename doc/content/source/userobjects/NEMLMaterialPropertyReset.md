# NEMLMaterialPropertyReset

!syntax description /UserObjects/NEMLMaterialPropertyReset

This user object resets the value of one or more internal state variables 
maintained by a NEML constitutive model above a certain coupled value of
temperature.  Users should have this 
object run at `execute_on = 'timestep_end'` to avoid affecting the convergence of
the MOOSE nonlinear solver.  This means that the NEML model will run normally during a
nonlinear iteration and return the correct Jacobian to MOOSE.  After solving the
nonlinear system then this object can freely alter the internal state of the model
without affecting overall convergence.

For non-scalar material properties the user should append an `_i` to the end of the
state variable name in NEML to get the `ith` component of the internal representation of
that object.  For example, symmetric tensor material properties like back stresses have
six components, from `_0` to `_5`, as they are stored in NEML as a Mandel notation vector.

One common use of this object is to anneal the NEML material response.  For example, 
the user could reset the internal hardening variables controlling the inelastic 
response of a NEML model above a certain temperature, effectively annealing out any 
prior work hardening developed by the model.

!syntax parameters /UserObjects/NEMLMaterialPropertyReset

!syntax inputs /UserObjects/NEMLMaterialPropertyReset

!syntax children /UserObjects/NEMLMaterialPropertyReset

