# ArrayFunctionIndexIC

!syntax description /ICs/ArrayFunctionIndexIC

All components of an array variable are initialized simultaneously using one function.
For component index `i` (0-based), this object evaluates the supplied function at
`x = i` (`y = 0`, `z = 0`) and uses that value for component `i`.

## Example input syntax

In this example, the array variable `u` has four components and is initialized with
`u(i) = 1.5 / (i + 1)`.

!listing test/tests/ics/array_function_index_ic/array_function_index_ic_test.i block=Functions ICs

!syntax parameters /ICs/ArrayFunctionIndexIC

!syntax inputs /ICs/ArrayFunctionIndexIC

!syntax children /ICs/ArrayFunctionIndexIC
