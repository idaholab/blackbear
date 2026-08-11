# CylinderAbsorbingBC

!syntax description /BCs/CylinderAbsorbingBC

## Description

The Lysmer-Kuhlemeyer absorbing boundary condition applies dynamic absorbing tractions at the truncated outer surface of a linear isotropic elastic domain to suppress reflections of outgoing mechanical waves. One BC instance is required per displacement component.

!alert! warning title=Dynamic only
This BC is valid only for dynamic (transient) analysis. The residual contains a time derivative term and collapses to zero in quasi-static loading. See [Limitations](#limitations).
!alert-end!

## Physics Derivation

The absorbing traction at the boundary is modeled as a dashpot-like impedance matching:

\begin{equation}
\mathbf{t} = -\rho c_p (\mathbf{n} \cdot \mathbf{v}) \mathbf{n} - \rho c_s (\mathbf{v} - (\mathbf{n} \cdot \mathbf{v}) \mathbf{n})
\end{equation}

where:
- $\rho$ is the mass density
- $c_p = \sqrt{\frac{\lambda + 2\mu}{\rho}}$ is the P-wave (dilatational) speed
- $c_s = \sqrt{\frac{\mu}{\rho}}$ is the S-wave (shear) speed
- $\mathbf{n}$ is the outward unit normal at the boundary quadrature point
- $\mathbf{v} = \frac{d\mathbf{u}}{dt}$ is the velocity at the boundary

The first term represents P-wave absorption (normal component of velocity), while the second term captures S-wave absorption (tangential component).

### Weak Form

The integrand applied to the test function $\psi$ is:

\begin{equation}
R_k = \psi_i \left[ -\rho c_p (\mathbf{n} \cdot \mathbf{v}) n_k - \rho c_s (v_k - (\mathbf{n} \cdot \mathbf{v}) n_k) \right]
\end{equation}

where $k$ denotes the displacement component this BC instance acts on (x, y, or z).

This factorizes cleanly into normal-projection (P-wave) and tangential (S-wave) parts, enabling straightforward Jacobian assembly.

## Parameters and Usage

!syntax parameters /BCs/CylinderAbsorbingBC
!syntax inputs /BCs/CylinderAbsorbingBC

The four required parameters are:

- `density` -- material property for mass density
- `shear_modulus` -- material property for $\mu$ (required to compute wave speeds)
- `lambda` -- material property for the first Lame parameter (required to compute P-wave speed)
- `displacements` -- the vector of three Cartesian displacement variables (in order: x, y, z)

The `component` enum specifies which displacement component (x, y, or z) this BC instance acts on. Declare one BC per component on the target boundary.

## Example Input File Syntax

!listing blackbear/test/tests/bcs/cylinder_absorbing/pwave_normal_incidence.i block=BCs

## Implementation Notes

### Jacobian

Hand-coded Jacobian terms include a diagonal block (derivative with respect to the time derivative of the same displacement component, with the implicit-Euler $1/\Delta t$ factor) and off-diagonal blocks (derivatives with respect to the time derivatives of the other displacement components, arising from the $(\mathbf{n} \cdot \mathbf{v})$ normal-projection coupling).

### Material Properties

Wave speeds are computed at each quadrature point from the material properties:

\begin{equation}
c_p = \sqrt{\frac{\lambda + 2\mu}{\rho}}, \quad c_s = \sqrt{\frac{\mu}{\rho}}
\end{equation}

These are re-evaluated at every time step, so the BC adapts if material properties vary spatially or temporally.

## Limitations

!alert! error title=Linear isotropic only
The derivation assumes linear elasticity with isotropic constitutive response. Anisotropic materials are out of scope; nonlinear (plastic, damaged) constitutive models invalidate the impedance match.
!alert-end!

!alert! warning title=No quasi-static validity
Without $\frac{d\mathbf{u}}{dt}$, the residual is identically zero. Quasi-static loading (where velocities are negligible) will not reflect waves appropriately. Use stress-free or fixed BCs for quasi-static termination.
!alert-end!

!alert! warning title=Smooth surfaces only
The BC assumes a locally smooth far-field boundary. Edges and corners of the truncated domain will reflect; the absorbing effect is most effective on smooth surfaces far from geometric singularities.
!alert-end!

!alert! warning title=Grazing-incidence degradation
Wave absorption degrades for grazing-incidence angles (waves approaching the boundary nearly parallel to the surface). Normal-incidence and moderate oblique angles are well absorbed; near-parallel incidence will reflect more energy.
!alert-end!

## Reference

[!cite](Lysmer1969)

!bibtex bibliography
