# ADClusterDynamicsNodalKernel / ClusterDynamicsNodalKernel

!syntax description /NodalKernels/ClusterDynamicsNodalKernel

## Description

The `ClusterDynamicsNodalKernel` implements the complete cluster dynamics rate equations for all cluster sizes (1 through N) in a +single array variable+. The rate coefficients can be supplied by either a simple power-law model or an interfacial-energy model based on cluster geometry and detailed balance for a single diffusing species.

!alert note title=Nodal Array Indexing
Array component index $i$ corresponds to cluster size $n = i+1$, i.e. monomer ($n=1$) array index is $i=0$; dimer ($n=2$) array index is $i=1$.  Largest cluster ($n=N$) array index is $i=N-1$.

### Rate Equations

+Component 0 (monomer, $n=1$):+

!equation
\frac{dC_1}{dt} = G_1 - k_s C_1 - 2\beta_1 C_1^2 - \sum_{n=2}^{N-1} \beta_n C_1 C_n + 2\alpha_2 C_2 + \sum_{n=3}^{N} \alpha_n C_n

where $G_1$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/generation)) is the monomer generation rate and $k_s$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/sink)) is the linear sink coefficient.

!alert note title=Generation and Sink Terms
$G_1$ and $k_s$ represent the production and loss of point defects, such as irradiation-generated vacancies and interstitials. They do not apply to chemical species and should be left at their default values of zero when modeling solute clustering such as Cu precipitation [!citep](bai2017).

+Components+ $i \geq 1$ +(cluster size+ $n = i+1 \geq 2$+):+

!equation
\frac{dC_n}{dt} = \dot{C}_n^{\text{in}} - \beta_n C_1 C_n + \alpha_{n+1} C_{n+1} - \alpha_n C_n

where the growth-in term is:

!equation
\dot{C}_n^{\text{in}} = \beta_{n-1} C_1 C_{n-1}, \qquad n \geq 2

For $n = 2$, this reduces to $\beta_1 C_1^2$ because $C_{n-1} = C_1$. Written in terms of the
flux between sizes $n$ and $n+1$,

!equation
J_n = \beta_n C_1 C_n - \alpha_{n+1} C_{n+1},

the cluster equations are $dC_n/dt = J_{n-1} - J_n$ and the monomer equation is
$dC_1/dt = G_1 - k_s C_1 - 2J_1 - \sum_{n=2}^{N-1} J_n$. Each dimer-forming reaction consumes two
monomers and produces one dimer, which is why the monomer equation carries $2\beta_1 C_1^2$ while the
dimer equation carries $\beta_1 C_1^2$ with no factor of $1/2$. With this form the total solute
content $\sum_n n C_n$ changes only through $G_1$ and $k_s$.

!alert note title=Largest Cluster Truncation
For the largest tracked cluster size $n = N$, the system is closed at the upper bound of the
truncated cluster space. In that case, the forward absorption term to an untracked $N+1$ cluster
is set to zero and there is no emission-in term from $N+1$:

!equation
\frac{dC_N}{dt} = \beta_{N-1} C_1 C_{N-1} - \alpha_N C_N

This avoids an unphysical loss of mass from the tracked system through the top cluster bin and
preserves mass within the truncated cluster space.

The rate coefficients depend on the selected [!param](/NodalKernels/ClusterDynamicsNodalKernel/rate_model). For `rate_model = simple`,

!equation
\beta_n = \beta_0 n^{1/3}, \qquad \alpha_n = \alpha_0 n^{1/3}

where $\beta_0$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/beta0)) and $\alpha_0$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/alpha0)) are user-supplied base coefficients.

In this simple model, [!param](/NodalKernels/ClusterDynamicsNodalKernel/atomic_volume) does not
enter the rate coefficients. It is only relevant if the same physical atomic volume is later used
by [ClusterTotalDensity.md] to convert the output from concentration-like units to `#/m^3`.

For `rate_model = interfacial_energy`, the absorption coefficient is computed from the cluster geometry and monomer diffusivity:

!equation
V_{at} = \text{atomic volume}, \qquad r_n = \left(\frac{3 n V_{at}}{4\pi}\right)^{1/3}

!equation
\beta_n = \frac{4\pi (r_1 + r_n) D_m}{V_{at}}

where $V_{at}$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/atomic_volume)) is the atomic volume and $D_m$ is the monomer diffusivity. The diffusivity is selected by [!param](/NodalKernels/ClusterDynamicsNodalKernel/diffusivity_model), which is used only when `rate_model = interfacial_energy`:

!equation
D_m = \text{monomer diffusivity}

for `diffusivity_model = constant`, where $D_m$ is supplied directly by [!param](/NodalKernels/ClusterDynamicsNodalKernel/monomer_diffusivity).

!equation
D_m = D_0 \exp\left(-\frac{Q}{k_B T}\right)

for `diffusivity_model = arrhenius`, where $D_0$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/D0)) is the diffusion prefactor and $Q$ is supplied directly in electron volts through [!param](/NodalKernels/ClusterDynamicsNodalKernel/Q_eV).

In both diffusivity modes, the computed monomer diffusivity is multiplied by
[!param](/NodalKernels/ClusterDynamicsNodalKernel/radiation_enhanced_factor), which defaults to
1.0 and may be used to represent radiation-enhanced transport without manually rescaling the input
diffusivity or diffusion prefactor.

The emission coefficient is then derived from the absorption coefficient by detailed balance using the cluster binding energy [!citep](bai2017):

!equation
\alpha_n = \beta_{n-1}\exp\left(-\frac{E_n^b}{k_B T}\right), \qquad n \geq 2

where the binding energy of a monomer to a cluster of size $n-1$ is related to the cluster formation
free energies $G_n$ by $E_n^b = G_1 + G_{n-1} - G_n$.

The binding energy is selected by [!param](/NodalKernels/ClusterDynamicsNodalKernel/binding_energy_model). This keeps the Cu precipitation benchmark available while allowing defect-cluster models to use energies that do not naturally come from an interface-energy picture. The available options are `interfacial_energy`, `capillary`, `binding_energy_table`, and `formation_energy_table`.

#### `binding_energy_model = interfacial_energy`

This option is the Cu precipitation form of [!cite](bai2017), which builds the binding energy from the formation enthalpy and entropy of the solute and the precipitate interfacial energy:

!equation
E_n^b = \Omega - T\Delta S - (36\pi)^{1/3}V_{at}^{2/3}\sigma\left[n^{2/3} - (n-1)^{2/3}\right]

where $T$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/temperature)) is the temperature, $\sigma$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/sigma)) is the interfacial energy, $V_{at}$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/atomic_volume)) is the atomic volume, $\Omega$ is supplied through [!param](/NodalKernels/ClusterDynamicsNodalKernel/Omega_kB_K) in $k_B \cdot K$, and $\Delta S$ is supplied through [!param](/NodalKernels/ClusterDynamicsNodalKernel/DeltaS_kB) in units of $k_B$.

Example:

```text
binding_energy_model = interfacial_energy
sigma = 0.37
Omega_kB_K = 6255.0
DeltaS_kB = 0.866
atomic_volume = 1.1782924e-29
```

#### `binding_energy_model = capillary`

This option uses a general capillary-type expression:

!equation
E_n^b = A - B\left[n^p - (n-1)^p\right]

where $A$ is the constant term, $B$ is the size-dependent coefficient, and $p$ is [!param](/NodalKernels/ClusterDynamicsNodalKernel/binding_energy_exponent). The model is intentionally flexible because different cluster-dynamics data sources report binding energies in different forms.

The constant term $A$ can be supplied directly in eV:

```text
binding_energy_constant_eV = 4.33
```

or computed from the user-supplied temperature:

!equation
A = \left(H_{k_BK} - T S_{k_B}\right) k_{B,\mathrm{eV}}

using [!param](/NodalKernels/ClusterDynamicsNodalKernel/binding_energy_enthalpy_kB_K) for $H_{k_BK}$ and [!param](/NodalKernels/ClusterDynamicsNodalKernel/binding_energy_entropy_kB) for $S_{k_B}$:

```text
binding_energy_enthalpy_kB_K = 6255.0
binding_energy_entropy_kB = 0.866
```

The coefficient $B$ can be supplied directly in eV:

```text
binding_energy_coefficient_eV = 5.76
```

or computed from an interfacial energy:

!equation
B = \frac{(36\pi)^{1/3}V_{at}^{2/3}\sigma}{e}

using [!param](/NodalKernels/ClusterDynamicsNodalKernel/sigma) and [!param](/NodalKernels/ClusterDynamicsNodalKernel/atomic_volume):

```text
sigma = 0.37
atomic_volume = 1.1782924e-29
```

The input should not provide both forms for the same term. For example, do not provide both [!param](/NodalKernels/ClusterDynamicsNodalKernel/binding_energy_coefficient_eV) and [!param](/NodalKernels/ClusterDynamicsNodalKernel/sigma), because both define $B$.

A direct fitted expression from a paper can be entered as:

```text
# E_b(n) = 4.33 - 5.76 * [n^(2/3) - (n - 1)^(2/3)]
binding_energy_model = capillary
binding_energy_constant_eV = 4.33
binding_energy_coefficient_eV = 5.76
binding_energy_exponent = 0.6666666666666666
```

The Cu benchmark can also be written with the same `capillary` model while keeping the interface energy visible:

```text
# E_b(n) = (H - T*S) * k_B
#        - (36*pi)^(1/3) * V_at^(2/3) * sigma * [n^p - (n - 1)^p]
binding_energy_model = capillary
binding_energy_enthalpy_kB_K = 6255.0
binding_energy_entropy_kB = 0.866
sigma = 0.37
atomic_volume = 1.1782924e-29
binding_energy_exponent = 0.6666666666666666
```

#### `binding_energy_model = binding_energy_table`

This option is used when the binding energies are already known directly. The user supplies the sequence:

!equation
\left[E_b(2), E_b(3), E_b(4), \ldots\right]

in eV through [!param](/NodalKernels/ClusterDynamicsNodalKernel/binding_energy_table_eV). Entry 0 corresponds to $E_b(2)$, entry 1 corresponds to $E_b(3)$, and so on.

Example:

```text
binding_energy_model = binding_energy_table
binding_energy_table_eV = '0.42 0.70 0.95 1.12 1.28'
```

#### `binding_energy_model = formation_energy_table`

This option is used when MD, DFT, or another source reports cluster formation energies rather than monomer binding energies. The user supplies the sequence:

!equation
\left[E_f(1), E_f(2), E_f(3), \ldots\right]

in eV through [!param](/NodalKernels/ClusterDynamicsNodalKernel/formation_energy_table_eV). The kernel converts formation energies to monomer-emission binding energies using:

!equation
E_b(n) = E_f(n-1) + E_f(1) - E_f(n)

Example:

```text
binding_energy_model = formation_energy_table
formation_energy_table_eV = '1.20 1.90 2.45 2.90 3.25'
```

When `rate_model = interfacial_energy`, [!param](/NodalKernels/ClusterDynamicsNodalKernel/atomic_volume) still enters the absorption coefficient through the cluster radius. If [ClusterTotalDensity.md] is used, the same physical atomic volume should also be used there to report cluster density in `#/m^3`. The grouped kernel uses the same `rate_model`, `diffusivity_model`, and `binding_energy_model` options, with the additional grouped-tail controls described in its input parameters.

### Intra-Variable Jacobian

Every cluster equation depends on the monomer concentration, and the monomer equation depends on
every cluster concentration. The non-AD version (`ClusterDynamicsNodalKernel`) therefore assembles
the full intra-variable Jacobian of the array variable, which has a dense first row and column plus a
tridiagonal band. The monomer row is:

!equation
\frac{\partial F_0}{\partial C_1} = k_s + 4\beta_1 C_1 + \sum_{n=2}^{N-1} \beta_n C_n,
\qquad
\frac{\partial F_0}{\partial C_n} = \beta_n C_1 - \mu_n \alpha_n,

where $\mu_2 = 2$, $\mu_n = 1$ otherwise, and the $\beta_n C_1$ term is absent for $n = N$. For cluster
rows $2 \leq n \leq N$:

!equation
\frac{\partial F_{n-1}}{\partial C_1} = -\left(\beta_{n-1} C_{n-1} - \beta_n C_n\right),
\quad
\frac{\partial F_{n-1}}{\partial C_{n-1}} = -\beta_{n-1} C_1,
\quad
\frac{\partial F_{n-1}}{\partial C_n} = \beta_n C_1 + \alpha_n,
\quad
\frac{\partial F_{n-1}}{\partial C_{n+1}} = -\alpha_{n+1},

where $\partial F_1/\partial C_1 = -(2\beta_1 C_1 - \beta_2 C_2)$ for the dimer, and the $\beta_N$ and
$\alpha_{N+1}$ terms are absent at the closed upper boundary.

!alert note title=Hash Table Matrix Assembly
These off-diagonal component entries lie outside the default sparsity pattern of an array variable,
so both the AD and non-AD kernels require `use_hash_table_matrix_assembly = true` in the `[Problem]`
block. Inserting the entries into the default preallocated pattern is prohibitively slow for large
cluster counts.

## Example Input Syntax

!listing test/tests/cluster_dynamics/cluster_dynamics_50_combined.i block=Problem NodalKernels id=cd_inputfile

!syntax parameters /NodalKernels/ClusterDynamicsNodalKernel

!syntax inputs /NodalKernels/ClusterDynamicsNodalKernel

!syntax children /NodalKernels/ClusterDynamicsNodalKernel

## See Also

- [ClusterAverageRadius.md] - Average cluster radius postprocessor
- [ClusterTotalDensity.md] - Total cluster density postprocessor
- [ClusterSizeConcentration.md] - Single cluster size concentration postprocessor
