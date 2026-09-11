[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

# 2x2x1 hex mesh spanning [-1,1] x [-1,1] x [0,1]. The interior column of
# nodes at x = y = 0 sits on the z-axis where, for cylindrical_axis = z,
# R = sqrt(x^2 + y^2) = 0. ExtraNodesetGenerator builds an explicit nodeset
# `axis_apex` containing those two nodes; assigning it as the constraint's
# `secondary` forces an on-axis node into _connected_nodes, which triggers
# the implementer's mooseError at the end of the constructor (computeAngles).
[Mesh]
  [block]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 2
    ny = 2
    nz = 1
    xmin = -1
    xmax = 1
    ymin = -1
    ymax = 1
    zmin = 0
    zmax = 1
    elem_type = HEX8
  []
  [axis_apex]
    type = ExtraNodesetGenerator
    input = block
    new_boundary = 'axis_apex'
    coord = '0 0 0; 0 0 1'
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    add_variables = true
    strain = SMALL
    incremental = false
  []
[]

[Constraints]
  [on_axis]
    type = CylindricalEqualValueBoundaryConstraint
    variable = disp_x
    displacements = 'disp_x disp_y disp_z'
    secondary = axis_apex
    cylindrical_axis = z
    penalty = 1.0e10
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 200.0e9
    poissons_ratio = 0.0
  []
  [stress]
    type = ComputeLinearElasticStress
  []
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
[]

[Outputs]
  exodus = false
[]
