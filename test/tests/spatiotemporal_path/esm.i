[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 20
    ny = 20
  []
  [solid]
    type = SubdomainBoundingBoxGenerator
    input = gmg
    block_id = 0
    block_name = solid
    bottom_left = '0 0 0'
    top_right = '1 1 0'
  []
  [liquid]
    type = SubdomainBoundingBoxGenerator
    input = solid
    block_id = 1
    block_name = liquid
    bottom_left = '0 0 0'
    top_right = '0.05 0.05 0'
  []
  use_displaced_mesh = false
[]

[Problem]
  solve = false
[]

[AuxVariables]
  [u]
  []
[]

[Functions]
  [path_x]
    type = PiecewiseLinear
    x = '3   4   5   6   7   8'
    y = '0 0.1 0.2 0.3 0.4 0.5'
  []
  [path_y]
    type = PiecewiseLinear
    x = '3   4   5   6   7   8'
    y = '0 0.1 0.2 0.3 0.4 0.5'
  []
[]

[SpatioTemporalPaths]
  [path]
    type = FunctionSpatioTemporalPath
    x = path_x
    y = path_y

    verbose = true
  []
[]

[UserObjects]
  [esm]
    type = SpatioTemporalPathElementSubdomainModifier
    path = 'path'
    radius = 0.2
    target_subdomain = 'liquid'
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  dt = 1
  end_time = 8
[]

[Outputs]
  exodus = true
[]
