[Mesh]
  type = SingleNodeMesh
[]

[Problem]
  use_hash_table_matrix_assembly = true
[]

[Variables]
  [clusters]
    family = LAGRANGE
    order = FIRST
    components = 5
  []
[]

[Functions]
  [index_fn]
    type = ParsedFunction
    expression = 'x*2'
  []
[]

[ICs]
  [clusters_ic]
    type = ArrayFunctionIndexIC
    variable = clusters
    function = index_fn
  []
[]

[NodalKernels]
  [clusters_dyn]
    type = ADClusterDynamicsNodalKernel
    variable = clusters
    generation = 0.03
    sink = 0.12
    beta0 = 0.14
    alpha0 = 0.05
  []
[]

[Executioner]
  type = Steady
  nl_rel_tol = 0.1
  nl_abs_tol = 10
[]

[Outputs]
  exodus=true
[]
