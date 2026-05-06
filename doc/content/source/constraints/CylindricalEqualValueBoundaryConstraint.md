# CylindricalEqualValueBoundaryConstraint

!syntax description /Constraints/CylindricalEqualValueBoundaryConstraint

## Description

`CylindricalEqualValueBoundaryConstraint` is the cylindrical-coordinate analogue of
[EqualValueBoundaryConstraint.md]. Where the parent class enforces equality of a
single scalar variable between a primary node and every secondary node on a
boundary, this class enforces equal *radial* displacement and zero *circumferential*
displacement on the two in-plane Cartesian displacement components simultaneously.
Axial displacement is left unconstrained.

This is the 3D extension of the established 2D trick of applying
[EqualValueBoundaryConstraint.md] to a single displacement component on a curved
arc to mimic an infinitely long cylinder; see
`modules/solid_mechanics/test/tests/anisotropic_elastoplasticity/hoop_strain_comparison.i`
for the 2D precedent in MOOSE.

The class extends [EqualValueBoundaryConstraint.md] by inheritance, reusing all of
the boundary-to-node-set construction, primary-node selection, distributed-mesh
ghosting, and adaptivity hooks. Only the residual and Jacobian are replaced:
the scalar-equality residual becomes a 2x2 rotation-matrix coupling between the
two in-plane displacement components.

### Formulation

Let the symmetry axis be specified by [!param](/Constraints/CylindricalEqualValueBoundaryConstraint/cylindrical_axis).
The two in-plane Cartesian displacement components are referred to below as
$u_{r_1}$ and $u_{r_2}$; the third is the axial component, which the constraint
does not touch. The convention follows the standard solid-mechanics
$(d+1)\%3,\,(d+2)\%3$ permutation: for axis $z$ the in-plane components are
$(u_x,\,u_y)$; for axis $x$ they are $(u_y,\,u_z)$; for axis $y$ they are
$(u_z,\,u_x)$. The
[!param](/Constraints/CylindricalEqualValueBoundaryConstraint/variable)
parameter must be set to the *first* in-plane component
($u_{r_1}$): `disp_x` for axis $z$, `disp_y` for axis $x$, `disp_z` for
axis $y$.

For the primary node $p$ and each secondary node $i$ at in-plane coordinates
$(r_{1,p},\,r_{2,p})$ and $(r_{1,i},\,r_{2,i})$, define the rotation angle
$\Delta\theta_i = \theta_i - \theta_p$ between primary and secondary about the
axis. The cosine and sine of this angle are computed directly from coordinates:

\begin{equation}
\cos(\Delta\theta_i) = \frac{r_{1,i}\,r_{1,p} + r_{2,i}\,r_{2,p}}{R_i\,R_p},
\qquad
\sin(\Delta\theta_i) = \frac{r_{2,i}\,r_{1,p} - r_{1,i}\,r_{2,p}}{R_i\,R_p},
\end{equation}

where $R_p$ and $R_i$ are the radial distances from the axis. The constraint
equations enforced (in penalty form) are:

\begin{equation}
u_{r_1,i} = u_{r_1,p}\cos(\Delta\theta_i) - u_{r_2,p}\sin(\Delta\theta_i),
\qquad
u_{r_2,i} = u_{r_1,p}\sin(\Delta\theta_i) + u_{r_2,p}\cos(\Delta\theta_i).
\end{equation}

This is equivalent to "every secondary node has the same radial displacement as
the primary, and zero circumferential displacement." The constraint admits
nodes at any wedge angle: $\cos$ and $\sin$ are evaluated per-secondary from
node coordinates, with no user-supplied angle. A full analytic Jacobian
(including off-diagonal coupling between $u_{r_1}$ and $u_{r_2}$ at the primary
node, accumulated over all secondaries) is provided.

A primary or secondary node at $R = 0$ (lying on the symmetry axis) produces
a clear `mooseError` at constraint construction; place no boundary nodes on
the axis.

!alert note title=Penalty formulation only
Unlike [EqualValueBoundaryConstraint.md], which supports both penalty and
Lagrange-multiplier formulations, this class implements the penalty
formulation only.

### Parameters

The required input parameters are:

- [!param](/Constraints/CylindricalEqualValueBoundaryConstraint/displacements) --
  the three Cartesian displacement variable names in order
  `'disp_x disp_y disp_z'`. The axis-permutation logic indexes into this list.
- [!param](/Constraints/CylindricalEqualValueBoundaryConstraint/secondary) --
  the boundary whose nodes form the secondary node set.
- [!param](/Constraints/CylindricalEqualValueBoundaryConstraint/penalty) --
  the penalty factor. The constraint is enforced strongly only when the
  penalty is large compared to the structural stiffness; a useful rule of
  thumb is `penalty` $\gtrsim 100 \, E$. With $E = 10^{8}$ Pa, a penalty of
  $10^{10}$ is sufficient; with $E = 10^{11}$ Pa the penalty should be
  $\gtrsim 10^{13}$. Values in the range `1e8` to `1e12` are typical, but
  the absolute number must be checked against the material stiffness in the
  problem at hand. Higher values enforce the constraint more strictly but
  can degrade conditioning.

Optional parameters:

- [!param](/Constraints/CylindricalEqualValueBoundaryConstraint/cylindrical_axis) --
  the symmetry axis (`x`, `y`, or `z`). Default: `z`.
- [!param](/Constraints/CylindricalEqualValueBoundaryConstraint/primary) --
  the primary node ID. If not supplied the first node of the secondary set is
  used (inherited from [EqualValueBoundaryConstraint.md]).
- [!param](/Constraints/CylindricalEqualValueBoundaryConstraint/primary_node_coord) --
  the primary node's coordinates, used as an alternative to
  [!param](/Constraints/CylindricalEqualValueBoundaryConstraint/primary). The
  two are mutually exclusive.

### Adding an axial constraint

The class does not constrain the axial displacement. To enforce uniform axial
displacement on the same surface as well, layer a separate
[EqualValueBoundaryConstraint.md] on the axial component:

```
[Constraints]
  [outer_axial]
    type      = EqualValueBoundaryConstraint
    variable  = disp_z
    secondary = outer_surface
    penalty   = 1e10
  []
[]
```

### Out of scope

- Automatic differentiation (no `AD` variant exists; the parent class is also
  non-AD).
- Lagrange-multiplier (kinematic) formulation.
- Non-Cartesian-aligned axes (arbitrary axis direction specified by two
  points).
- Symmetry boundary conditions on the flat cut faces of a wedge sector --
  those are the user's responsibility. Cartesian-aligned cuts can use
  `DirichletBC`; non-axis-aligned cuts can use the
  `[BCs][InclinedNoDisplacementBC]` action from the solid_mechanics module.
  The same penalty-vs-stiffness rule of thumb applies to the inclined BC's
  own penalty.

## Example Input File Syntax

### Quarter-cylinder, z-axis

A 1/4 cylindrical wedge with the symmetry axis along $z$. The constraint is
applied to the outer curved surface. Cut-face symmetry boundary conditions
(`disp_y = 0` on the $\theta = 0$ face and `disp_x = 0` on the
$\theta = \pi/2$ face) are applied separately as `DirichletBC`.

```
[Constraints]
  [outer_radial]
    type          = CylindricalEqualValueBoundaryConstraint
    variable      = disp_x
    displacements = 'disp_x disp_y disp_z'
    secondary     = outer_surface
    penalty       = 1e10
    cylindrical_axis = z
  []
[]
```

### Full-ring, z-axis

A full 360-degree annular cross section. No cut-face symmetry boundary
conditions are needed; the constraint alone closes the kinematics.

```
[Constraints]
  [outer_radial]
    type          = CylindricalEqualValueBoundaryConstraint
    variable      = disp_x
    displacements = 'disp_x disp_y disp_z'
    secondary     = outer_surface
    penalty       = 1e10
    cylindrical_axis = z
  []
[]
```

### Full-ring, x-axis

A full 360-degree ring oriented so that the symmetry axis is the Cartesian
$x$ direction (typically built by rotating an
`AnnularMeshGenerator` ring into the $yz$-plane and extruding along $+x$).
For
[!param](/Constraints/CylindricalEqualValueBoundaryConstraint/cylindrical_axis)`=x`
the in-plane components are $(u_y,\,u_z)$, so
[!param](/Constraints/CylindricalEqualValueBoundaryConstraint/variable) must be
the $y$ displacement -- the first in-plane component for that axis.

```
[Constraints]
  [outer_radial]
    type          = CylindricalEqualValueBoundaryConstraint
    variable      = disp_y
    displacements = 'disp_x disp_y disp_z'
    secondary     = outer_surface
    penalty       = 1e10
    cylindrical_axis = x
  []
[]
```

### Wedge sector with one inclined cut face

For a wedge whose cut faces are not axis-aligned (for example, a 60-degree
or 120-degree sector starting at $\theta = 0$), the cylindrical constraint
on the curved outer surface is paired with `DirichletBC` on the
axis-aligned cut face and `[BCs][InclinedNoDisplacementBC]` on the
inclined cut face:

```
[BCs]
  [sym_theta0]
    type     = DirichletBC
    variable = disp_y
    boundary = dmin
    value    = 0
  []
  [InclinedNoDisplacementBC]
    [sym_inclined]
      boundary = dmax
      penalty  = 1e10
    []
  []
[]
```

The penalty on the inclined BC must be of comparable magnitude to the
constraint's own penalty; mismatched stiffnesses lead to one or the other
being effectively inactive (see the rule of thumb in
[#Parameters](#parameters)).

## Validation

The regression test suite validates the radial displacement on the outer
surface of an internally-pressurized thick-walled cylinder against the
closed-form Lame thick-walled cylinder solution

\begin{equation}
u_r(r) = \frac{p\,a^2}{E\,(b^2 - a^2)}\left[(1 - \nu)\,r + (1 + \nu)\,\frac{b^2}{r}\right],
\end{equation}

where $a$ and $b$ are the inner and outer radii and $p$ is the internal
pressure. See `Theory of Elasticity`, Timoshenko and Goodier, for the
derivation.

The suite covers a 90-degree wedge with Cartesian-aligned cuts, 60-degree
and 120-degree wedges with one inclined cut, a 90-degree wedge rotated
30-degrees so both cuts are inclined, and full 360-degree rings with the
symmetry axis along both $z$ and $x$. All match the analytical Lame
solution to within ~2% on the chosen mesh resolution.

Two implementation details are required for the analytical match at finite
displacement:

- The internal `Pressure` BC must be applied to the reference (undisplaced)
  configuration via `use_displaced_mesh = false` on the Pressure sub-block.
  The default follower-load behavior overshoots Lame proportionally to
  $(u/r)^2$ because the deformed inner surface is larger than the
  reference.
- The penalty on any inclined cut-face BC must be comparable in magnitude
  to the cylindrical constraint's own penalty (see the
  penalty-vs-stiffness rule in [#Parameters](#parameters)).

A separate regression test exercises a non-axisymmetric (mode-2) pressure
load $p(\theta) = p_0 \,(1 + 0.5\,\cos 2\theta)$ on the inner surface to
demonstrate that the constraint actively suppresses non-axisymmetric
deformation modes on the constrained outer surface: with the constraint
active, four cardinal angular probes on the outer surface report
identical $u_r$; without the constraint, those probes spread by a factor
of three to four.

!syntax parameters /Constraints/CylindricalEqualValueBoundaryConstraint

!syntax inputs /Constraints/CylindricalEqualValueBoundaryConstraint

!syntax children /Constraints/CylindricalEqualValueBoundaryConstraint
