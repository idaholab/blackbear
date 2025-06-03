# ADRebarBondSlipConstraint

!syntax description /Constraints/ADRebarBondSlipConstraint

This version of `RebarBondSlipConstraint` uses automatic differentiation, see [RebarBondSlipConstraint.md] for information on formulation.  `ADRebarBondSlipConstraint` has a more accurate Jacobian and may help if convergence problems are experienced with `RebarBondSlipConstraint`, this is especially true for the [!param](/Constraints/ADRebarBondSlipConstraint/bondslip_model)=concrete_rebar_model which has a nonlinear force displacement response.

!alert warning title=`ADRebarBondSlipConstraint` only works for PENALTY formulation
[ADEqualValueEmbeddedConstraint.md] provides an explaination on why KINEMATIC formulation does not work.

!syntax parameters /Constraints/ADRebarBondSlipConstraint

!syntax inputs /Constraints/ADRebarBondSlipConstraint

!syntax children  /Constraints/ADRebarBondSlipConstraint
