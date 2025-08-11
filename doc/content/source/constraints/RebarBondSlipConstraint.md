# RebarBondSlipConstraint

!syntax description /Constraints/RebarBondSlipConstraint

`RebarBondSlipConstraint` implements a node-to-element constraint  designed to apply the bond-slip relations between concrete and reinforcement bars. The rebar node is always [!param](/Constraints/RebarBondSlipConstraint/secondary) with the concrete element being the [!param](/Constraints/RebarBondSlipConstraint/primary).  It uses a simplistic bond-slip model that assigns bond stress based on the slip values calculated as the relative displacement between concrete and rebar.

Two bond-slip models have been implemented.  The default model selected by [!param](/Constraints/RebarBondSlipConstraint/bondslip_model)=`concrete_rebar_model` is the nonlinear model from [!cite](bondlsip_adina_1985) fit to experimental concrete rebar data and given as

\begin{equation}
  \sigma_s =
  \begin{cases}
     \sigma_{max} \left[ 0.5 \left(\frac{\Delta}{\Delta_T}\right) - 4.5 \left(\frac{\Delta}{\Delta_T}\right)^2 + 1.4 \left(\frac{\Delta}{\Delta_T}\right)^3 \right] & \Delta < \Delta_T  \\
     1.9 \sigma_{max} & \Delta \geq \Delta_T
  \end{cases}
\end{equation}

where, $\sigma_s$ is the bondstress, $\sigma_{max}$ is the [!param](/Constraints/RebarBondSlipConstraint/max_bondstress) related to the compressive strength of the concrete, $\Delta$ is the slip calculated as the relative displacement between the concrete and rebar in the axial direction of the rebar, and $\Delta_T$ is the [!param](/Constraints/RebarBondSlipConstraint/transitional_slip_value) at which the maximum bondstress is reached.  The [!param](/Constraints/RebarBondSlipConstraint/rebar_radius) is also required by this model to convert the bondstress to a constraint force.  The Jacobian for the nonlinear model is not exact, leading to convergence issues that can be be solved with [ADRebarBondSlipConstraint.md].

The second bondslip model is an elastic perfect plastic version of the nonlinear model.  Initial slip of the rebar-concrete bond is a linear function of stress.  Linear slip continues up to [!param](/Constraints/RebarBondSlipConstraint/max_bondstress) and [!param](/Constraints/RebarBondSlipConstraint/transitional_slip_value).  After `max_bondstress` is reached, additional slip occurs at a constant force.  Unloading follows teh slope of the linear loading curve. This model is selected by [!param](/Constraints/RebarBondSlipConstraint/bondslip_model)=`elastic_perfect_plastic_model`.

The bond slip model outputs for [!param](/Constraints/RebarBondSlipConstraint/output_axial_slip), [!param](/Constraints/RebarBondSlipConstraint/output_axial_force), and [!param](/Constraints/RebarBondSlipConstraint/output_axial_plastic_slip) can be written to `AuxVariables`.  [!param](/Constraints/RebarBondSlipConstraint/output_axial_slip) is the total amount of slip in the constraint and [!param](/Constraints/RebarBondSlipConstraint/output_axial_plastic_slip) is the plastic component of the slip.

## Example Input File Syntax

!listing rebar_bondslip/RCBeam_elastic_constraint.i block=Constraints/rebar_x

!syntax parameters /Constraints/RebarBondSlipConstraint

!syntax inputs /Constraints/RebarBondSlipConstraint

!syntax children  /Constraints/RebarBondSlipConstraint

!bibtex bibliography
