# Shared by manual.i and csv.i, which differ only in how the path vertices are given.

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

[Materials]
  [heat_source]
    type = ADMovingEllipsoidalHeatSource
    spatiotemporal_path = path
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
