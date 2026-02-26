# ADClusterDynamicsNodalKernel / ClusterDynamicsNodalKernel

!syntax description /NodalKernels/ClusterDynamicsNodalKernel

## Description

The `ClusterDynamicsNodalKernel` implements the complete cluster dynamics rate equations for all cluster sizes (1 through N) in a +single array variable+.

!alert note title=Nodal Array Indexing
Array component index $i$ corresponds to cluster size $n = i+1$, i.e. monomer ($n=1$) array index is $i=0$; dimer ($n=2$) array index is $i=1$.  Largest cluster ($n=N$) array index is $i=N-1$.

### Rate Equations

+Component 0 (monomer, $n=1$):+

!equation
\frac{dC_1}{dt} = G_1 - k_s C_1 - 2\beta_1 C_1^2 - \sum_{n=2}^{N} \beta_n C_1 C_n + 2\alpha_2 C_2 + \sum_{n=3}^{N} \alpha_n C_n

where $G_1$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/generation)) is the monomer generation rate,  $k_s$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/sink)) is the  Linear sink coefficient

+Components+ $i \geq 1$ +(cluster size+ $n = i+1 \geq 2$+):+

!equation
\frac{dC_n}{dt} = \dot{C}_n^{\text{in}} - \beta_n C_1 C_n + \alpha_{n+1} C_{n+1} - \alpha_n C_n

where the growth-in term is:

!equation
\dot{C}_n^{\text{in}} = \begin{cases} \tfrac{1}{2}\beta_1 C_1^2 & n = 2 \\ \beta_{n-1} C_1 C_{n-1} & n > 2 \end{cases}

Absorption and emission rate coefficients scale with the cluster surface area:

!equation
\beta_n = \beta_0 n^{1/3}, \qquad \alpha_n = \alpha_0 n^{1/3}

where $\beta_0$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/beta0)) and $\alpha_0$ ([!param](/NodalKernels/ClusterDynamicsNodalKernel/alpha0)) are base coefficients.

### Intra-Variable Jacobian

The non-AD version (`ClusterDynamicsNodalKernel`) provides a full analytical Jacobian via intra-variable coupling using the `setJacobian` framework API.

+Monomer row ($i=0$):+

Diagonal entry (coupling of the monomer equation to $C_1$):

!equation
\frac{\partial F_0}{\partial C_1} = k_s + 4\beta_1 C_1 + \sum_{n \geq 2} \beta_n C_n

Off-diagonal entries (coupling of the monomer equation to $C_n$ for $n \geq 2$):

!equation
\frac{\partial F_0}{\partial C_n} = \beta_n C_1 - \mu_n \alpha_n, \quad n \geq 2, \quad \mu_n = \begin{cases} 2 & n = 2 \\ 1 & n > 2 \end{cases}

+Cluster rows (+$i \geq 1$, $n = i+1$+):+

Coupling to the monomer column:

!equation
\frac{\partial F_i}{\partial C_1} = -\beta_{n-1} C_{n-1} - \beta_n C_n + \begin{cases} \beta_1 C_1 & n = 2 \\ \beta_{n-1} C_{n-1} & n > 2 \end{cases}

Lower-diagonal entry (coupling to cluster $n-1$, for $n > 2$):

!equation
\frac{\partial F_i}{\partial C_{n-1}} = -\beta_{n-1} C_1

Diagonal entry:

!equation
\frac{\partial F_i}{\partial C_n} = \beta_n C_1 + \alpha_n

Upper-diagonal entry (coupling to cluster $n+1$, for $n+1 \leq N$):

!equation
\frac{\partial F_i}{\partial C_{n+1}} = -\alpha_{n+1}

!alert note title=Problem Requirement
The intra-variable Jacobian capability requires `use_hash_table_matrix_assembly = true` see below input file [!ref](cd_inputfile)

## Example Input Syntax

!listing test/tests/nodalkernels/cluster_dynamics/cluster_dynamics_50_combined.i block=Problem NodalKernels id=cd_inputfile

!syntax parameters /NodalKernels/ClusterDynamicsNodalKernel

!syntax inputs /NodalKernels/ClusterDynamicsNodalKernel

!syntax children /NodalKernels/ClusterDynamicsNodalKernel

## See Also

- [ClusterAverageRadius.md] - Average cluster radius postprocessor
- [ClusterTotalDensity.md] - Total cluster density postprocessor
- [ClusterSizeConcentration.md] - Single cluster size concentration postprocessor
