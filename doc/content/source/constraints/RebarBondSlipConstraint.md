# ADRebarBondSlipConstraint / RebarBondSlipConstraint

!syntax description /Constraints/ADRebarBondSlipConstraint

`ADRebarBondSlipConstraint` implements a node-to-element constraint  designed to apply the bond-slip relations between concrete and reinforcement bars. The rebar node is always [!param](/Constraints/ADRebarBondSlipConstraint/secondary) with the concrete element being the [!param](/Constraints/ADRebarBondSlipConstraint/primary).  It uses a bond-slip model that assigns bond stress based on the slip values calculated as the relative displacement between concrete and rebar.

The `ADRebarBondSlipConstraint` is recommended as the nonAD version, `RebarBondSlipConstraint`, as currently implemented does not have an exact Jacobian, potentially leading to convergence issues that can be be resolved by using the `ADRebarBondSlipConstraint`.  There is also very little overhead or input file syntax change required for using `ADRebarBondSlipConstraint` as the AD terms are limited to the constraint blocks as shown in [list:adconstraint].

!alert warning title=`ADRebarBondSlipConstraint` only works for PENALTY formulation
[EqualValueEmbeddedConstraint.md] provides an explaination on why KINEMATIC formulation does not work.

Two bond-slip models have been implemented.  The default model selected by [!param](/Constraints/ADRebarBondSlipConstraint/bondslip_model)=`concrete_rebar_model` is the nonlinear model from [!cite](bondlsip_adina_1985) fit to experimental concrete rebar data and given as

\begin{equation}
  \sigma_s =
  \begin{cases}
     \sigma_{max} \left[ 0.5 \left(\frac{\Delta}{\Delta_T}\right) - 4.5 \left(\frac{\Delta}{\Delta_T}\right)^2 + 1.4 \left(\frac{\Delta}{\Delta_T}\right)^3 \right] & \Delta < \Delta_T  \\
     1.9 \sigma_{max} & \Delta \geq \Delta_T
  \end{cases}
\end{equation}

where, $\sigma_s$ is the bond stress, $\sigma_{max}$ is the [!param](/Constraints/ADRebarBondSlipConstraint/max_bond_stress) related to the compressive strength of the concrete, $\Delta$ is the slip calculated as the relative displacement between the concrete and rebar in the axial direction of the rebar, and $\Delta_T$ is the [!param](/Constraints/ADRebarBondSlipConstraint/transitional_slip_value) at which the maximum bond stress is reached.  The [!param](/Constraints/ADRebarBondSlipConstraint/rebar_radius) is also required by this model to convert the bond stress to a constraint force.

The second bond-slip model is an elastic perfect plastic version of the nonlinear model.  The initial slip of the rebar-concrete bond is a linear function of stress.  Linear slip continues up to [!param](/Constraints/ADRebarBondSlipConstraint/max_bond_stress) and [!param](/Constraints/ADRebarBondSlipConstraint/transitional_slip_value).  After `max_bond_stress` is reached, additional slip occurs at a constant force.  Unloading follows the slope of the linear loading curve. This model is selected by [!param](/Constraints/RebarBondSlipConstraint/bondslip_model)=`elastic_perfect_plastic_model`.

Information on the bond-slip model's behavior can be written to `AuxVariable` through the [!param](/Constraints/ADRebarBondSlipConstraint/output_axial_slip), [!param](/Constraints/ADRebarBondSlipConstraint/output_axial_force), and [!param](/Constraints/ADRebarBondSlipConstraint/output_axial_plastic_slip) parameters.  [!param](/Constraints/ADRebarBondSlipConstraint/output_axial_slip) is the total amount of slip in the constraint and [!param](/Constraints/ADRebarBondSlipConstraint/output_axial_plastic_slip) is the plastic component of the slip.

## Example Input File Syntax

!listing rebar_bondslip/RCBeam_elastic_constraint_45Deg.i id=list:adconstraint block=Constraints/rebar_x caption=Example usage of ADRebarBondSlipConstraint. Note the AD terms are limited to the constraint and do not show up in other blocks.

!syntax parameters /Constraints/ADRebarBondSlipConstraint

!syntax inputs /Constraints/ADRebarBondSlipConstraint

!syntax children  /Constraints/ADRebarBondSlipConstraint

!bibtex bibliography
