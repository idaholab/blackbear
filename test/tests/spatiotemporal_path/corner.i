# L-shaped path: moves in +x for 0 <= t <= 1, then in +y for 1 <= t <= 2.
# The heat source direction (and so the ellipsoid orientation) jumps at the corner, t = 1,
# and is zero outside the path's time range (t > 2).
[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 10
  []
[]

[Problem]
  solve = false
[]

[SpatioTemporalPaths]
  [path]
    type = SpatioTemporalPath
    t = '0 1 2'
    x = '0 1 1'
    y = '0 0 1'
  []
[]

[Materials]
  [heat_source]
    type = ADMovingEllipsoidalHeatSource
    spatiotemporal_path = path
    power = 1
    a = 0.4
    b = 0.2
  []
[]

[Postprocessors]
  # The largest tangential distance (measured along the direction of travel) drops back to its
  # maximum when the direction turns at t = 1, and is 0 after the path ends at t = 2.
  [tangential_max]
    type = ElementExtremeMaterialProperty
    mat_prop = heat_source_tangential_distance
    value_type = max
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [normal_max]
    type = ElementExtremeMaterialProperty
    mat_prop = heat_source_normal_distance
    value_type = max
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  dt = 0.25
  end_time = 2.5
[]

[Outputs]
  csv = true
[]
