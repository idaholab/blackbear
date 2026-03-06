[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 1
[]

[Problem]
  use_hash_table_matrix_assembly = true
[]

[Variables]
  [clusters]
    family = LAGRANGE
    order = FIRST
    components = 50
  []
[]

[Functions]
  [index_fn]
    type = PiecewiseConstant
    x = '0 0.9'
    y = '1 0.02'
    direction = left
  []
[]

[ICs]
  [u_ic]
    type = ArrayFunctionIndexIC
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
    type = ADClusterDynamicsNodalKernel
    variable = clusters
    generation = 0.03
    sink = 0.12
    beta0 = 0.14
    alpha0 = 0.05
  []
[]

[Postprocessors]
  [avg_cluster_radius]
    type = ClusterAverageRadius
    clusters = clusters
    r1 = 1.0
    n_minimum = 2
  []
  [monomer_concentration]
    type = ClusterSizeConcentration
    clusters = clusters
    n_size = 1
  []
  [total_cluster_density]
    type = ClusterTotalDensity
    clusters = clusters
    n_minimum = 1
  []
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  nl_rel_tol = 1e-9
  nl_abs_tol = 1e-12
  dt = 0.1
  num_steps = 5
[]

[Outputs]
  csv = true
[]
