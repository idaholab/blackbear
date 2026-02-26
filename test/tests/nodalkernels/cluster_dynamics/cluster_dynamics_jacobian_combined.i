# Jacobian test for non-AD ClusterDynamicsNodalKernel
# This test verifies that the intra-variable analytical Jacobian matches the
# finite-difference Jacobian computed by PETSc.
# All cluster sizes (1 through N) are in a single array variable; index 0 = monomer.

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 2
[]

[Problem]
  use_hash_table_matrix_assembly = true
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

[Executioner]
  type = Transient
  solve_type = NEWTON
  petsc_options_iname = '-snes_test_err'
  petsc_options_value = '1e-8'
  dt = 0.01
  num_steps = 1
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-8
[]

[Outputs]
  console = true
  exodus = false
  csv = false
[]
