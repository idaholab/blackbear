# Demo input (NOT a regression test): paired with asym_pressure_z_axis.i.
# Same quadrupole pressure on the inner surface but with the cylindrical
# constraint removed. The outer surface freely picks up mode-2 deformation;
# u_r at the four cardinal probes spreads from ~0.09 to ~0.34 m
# (3.8x ratio). Run manually to compare against the constrained case.
#
# Anchors: disp_y = 0 at (1, 0, 0) and disp_x = 0 at (0, 1, 0) kill
# translation and rotation about z; bottom face disp_z = 0 kills the
# remaining rigid body modes.
[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  [annulus]
    type = AnnularMeshGenerator
    nr = 12
    nt = 64
    rmin = 0.5
    rmax = 1.0
    dmin = 0
    dmax = 360
  []
  [extrude]
    type = MeshExtruderGenerator
    input = annulus
    extrusion_vector = '0 0 1.0'
    num_layers = 4
    bottom_sideset = 'bottom'
    top_sideset = 'top'
  []
  # Anchor: pin one node to remove translational null space and another
  # to remove the residual rotation about z.
  [pin_anchor]
    type = ExtraNodesetGenerator
    input = extrude
    new_boundary = 'anchor'
    coord = '1 0 0'
  []
  [pin_rotation]
    type = ExtraNodesetGenerator
    input = pin_anchor
    new_boundary = 'rotation_pin'
    coord = '0 1 0'
  []
[]

[Functions]
  [cos_pressure]
    type = ParsedFunction
    expression = '1.5e7 * (1 + 0.5*cos(2*atan2(y, x)))'
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    add_variables = true
    strain = SMALL
    incremental = false
  []
[]

[BCs]
  [sym_bottom]
    type = DirichletBC
    variable = disp_z
    boundary = bottom
    value = 0
  []
  [anchor_disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = anchor
    value = 0
  []
  [rotation_disp_x]
    type = DirichletBC
    variable = disp_x
    boundary = rotation_pin
    value = 0
  []
  [Pressure]
    [internal_pressure]
      boundary = rmin
      function = cos_pressure
      use_displaced_mesh = false
    []
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1.0e8
    poissons_ratio = 0.0
  []
  [stress]
    type = ComputeLinearElasticStress
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-10
[]

[Postprocessors]
  [u_r_theta0]
    type = PointValue
    variable = disp_x
    point = '1.0 0 0'
  []
  [u_r_theta90]
    type = PointValue
    variable = disp_y
    point = '0 1.0 0'
  []
  [u_r_theta180_signed]
    type = PointValue
    variable = disp_x
    point = '-1.0 0 0'
  []
  [u_r_theta270_signed]
    type = PointValue
    variable = disp_y
    point = '0 -1.0 0'
  []
[]

[Outputs]
  exodus = false
  csv = true
[]
