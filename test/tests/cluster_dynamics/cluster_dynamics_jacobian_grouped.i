# Jacobian test for non-AD ClusterDynamicsGroupedNodalKernel
# This test verifies that the intra-variable analytical Jacobian, including the
# coupling of grouped L0/L1 components through the inter-bin fluxes, matches the
# finite-difference Jacobian computed by PETSc.

[Mesh]
  type = SingleNodeMesh
  position = '1 1 1'
[]

[Problem]
  use_hash_table_matrix_assembly = true
[]

[GlobalParams]
  num_cluster_sizes = 30
  use_grouping = true
  explicit_max = 6
  group_spacing = linear
  group_linear_width = 4
  group_geometric_ratio = 1.5
  enforce_group_nonnegative = true
  group_nonnegative_tolerance_factor = 0.0
[]

[GroupedVariables]
  [clusters]
    family = LAGRANGE
    order = FIRST
  []
[]

[Functions]
  [index_fn]
    # smooth decay with a step inside the grouped tail so that some bins are slope limited
    type = ParsedFunction
    expression = 'if(x < 15.5, 1.5 * exp(-0.2 * x), 1e-6)'
  []
[]

[ICs]
  [clusters_ic]
    type = GroupedArrayFunctionIndexIC
    variable = clusters
    function = index_fn
  []
[]

[NodalKernels]
  [clusters_dot]
    type = ArrayTimeDerivativeNodalKernel
    variable = clusters
  []
  [clusters_dyn]
    type = ClusterDynamicsGroupedNodalKernel
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
  dt = 0.01
  num_steps = 1
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-8
[]

[Outputs]
  console = true
[]
