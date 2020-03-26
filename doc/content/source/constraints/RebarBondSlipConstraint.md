# RebarBondSlipConstraint

!syntax description /Constraints/RebarBondSlipConstraint

`RebarBondSlipConstraint` implements a node-to-element constraint  designed to apply the bond-slip relations between concrete and reinforcement bars. It uses a simplistic bond-slip model that assigns bond stress based on the slip values calculated as the relative displacement between concrete and rebar:
\begin{equation}
  \begin{aligned}
    \sigma_s = & \sigma_{max} \left[ 0.5 \left(\frac{\Delta}{\Delta_1}\right) \, - \, 4.5 \, \left(\frac{\Delta}{\Delta_1}\right)^2 \, + \, 1.4 \, \left(\frac{\Delta}{\Delta_1}\right)^3 \right] \;  \mathrm{for} \, \Delta < \Delta_1 \, \,  \\
    = & 1.9 \, \sigma_{max} \;  \mathrm{for}  \, \Delta >= \Delta_1
  \end{aligned}
\end{equation}
Here, $\sigma_s$ is the bondstress, $\sigma_{max}$ is the maximum bondstress related to the compressive strength of the concrete, $\Delta$ is the slip calculated as the relative displacement between the concrete and rebar in the axial direction of the rebar, and $\Delta_1$ is the slip magnitude at which the maximum bondstress is reached. This model is similar to what was implemented in [!cite](bondlsip_adina_1985).

## Example Input File Syntax

!listing rebar_bondslip/RCBeam_constraint.i block=Constraints

!syntax parameters /Constraints/RebarBondSlipConstraint

!syntax inputs /Constraints/RebarBondSlipConstraint

!syntax children  /Constraints/RebarBondSlipConstraint

!bibtex bibliography
