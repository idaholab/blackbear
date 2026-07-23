[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 1
[]

[GlobalParams]
  num_cluster_sizes = 50
  use_grouping = true
  explicit_max = 10
  group_spacing = linear
  group_linear_width = 5
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
    type = PiecewiseConstant
    x = '0 0.9'
    y = '1 0.02'
    direction = left
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
    type = ADClusterDynamicsGroupedNodalKernel
    variable = clusters
    generation = 0.03
    sink = 0.12
    beta0 = 0.14
    alpha0 = 0.05
  []
[]

[Postprocessors]
  [avg_cluster_radius]
    type = ClusterAverageRadiusGrouped
    clusters = clusters
    r1 = 1.0
    n_minimum = 2
  []
  [monomer_concentration]
    type = ClusterSizeConcentrationGrouped
    clusters = clusters
    n_size = 1
  []
  [total_cluster_density]
    type = ClusterTotalDensityGrouped
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
