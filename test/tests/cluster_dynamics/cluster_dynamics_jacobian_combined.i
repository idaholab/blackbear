# Jacobian test for non-AD ClusterDynamicsNodalKernel
# This test verifies that the intra-variable analytical Jacobian matches the
# finite-difference Jacobian computed by PETSc.
# All cluster sizes (1 through N) are in a single array variable; index 0 = monomer.

[Mesh]
  type = SingleNodeMesh
  position = '1 1 1'
[]

[Problem]
  # Required for the intra-variable Jacobian; keep the fixed sparsity after the first assembly
  use_hash_table_matrix_assembly = true
  restore_original_nonzero_pattern = false
[]

[Variables]
  [clusters]
    family = LAGRANGE
    order = FIRST
    components = 11
  []
[]

[ICs]
  [clusters_ic]
    type = ArrayConstantIC
    variable = clusters
    # index 0 = monomer (size 1), indices 1-10 = clusters sizes 2-11
    value = '1.5 0.05 0.04 0.03 0.025 0.02 0.015 0.01 0.008 0.006 0.004'
  []
[]

[NodalKernels]
  [clusters_dot]
    type = ArrayTimeDerivativeNodalKernel
    variable = clusters
  []
  [clusters_dyn]
    type = ClusterDynamicsNodalKernel
    variable = clusters
    generation = 0.05
    sink = 0.15
    beta0 = 0.20
    alpha0 = 0.08
  []
[]

[Preconditioning]
  # Avoid the default full coupling matrix, which is O(N^2) in the number of array components
  [smp]
    type = SMP
    full = false
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  # Reorder so that factoring the dense monomer row and column does not cost O(N^2)
  petsc_options_iname = '-pc_type -pc_factor_mat_ordering_type'
  petsc_options_value = 'ilu      rcm'
  dt = 0.01
  num_steps = 1
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-8
[]

[Outputs]
  console = true
[]
