[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 20
    ny = 20
  []
  use_displaced_mesh = false
[]

[Problem]
  solve = false
[]

[SpatioTemporalPaths]
  [path]
    type = PiecewiseLinearSpatioTemporalPath
    t = '3   4   5   6   7   8'
    x = '0 0.1 0.2 0.3 0.4 0.5'
    y = '0 0.1 0.2 0.3 0.4 0.5'
    verbose = true
  []
[]

[Materials]
  [heat_source]
    type = ADMovingEllipsoidalHeatSource
    path = path
    power = 1
    efficiency = 1
    scale = 1
    a = 0.4
    b = 0.2
    outputs = exodus
  []
[]

[Executioner]
  type = Transient
  dt = 0.5
  end_time = 10
[]

[Outputs]
  exodus = true
[]
