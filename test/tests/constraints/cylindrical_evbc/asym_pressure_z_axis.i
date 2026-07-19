# Asymmetric-pressure validation: quadrupole pressure
# p(theta) = p_0 * (1 + 0.5 * cos(2 theta)) on the inner surface drives a
# mode-2 deformation that the cylindrical constraint must suppress on the
# outer surface. All four angular probes on rmax should give the same u_r,
# matching the cycle-averaged Lame value.
#
# Mutation rationale: removing or weakening the constraint allows the outer
# surface to pick up the mode-2 deformation; the four probe values diverge
# by a factor of 3 or more. See the paired demo input
# `asym_pressure_z_axis_no_constraint.i` for the without-constraint solution.
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
  [pin_primary]
    type = ExtraNodesetGenerator
    input = extrude
    new_boundary = 'primary_pin'
    coord = '1 0 0'
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
  [pin_primary_disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = primary_pin
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

[Constraints]
  [outer_radial]
    type = CylindricalEqualValueBoundaryConstraint
    variable = disp_x
    displacements = 'disp_x disp_y disp_z'
    secondary = rmax
    cylindrical_axis = z
    penalty = 1.0e10
    primary_node_coord = '1 0 0'
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
  # u_r at 4 outer-surface probes around the ring.
  # u_r at theta=0   = +disp_x at (1, 0, 0)
  # u_r at theta=90  = +disp_y at (0, 1, 0)
  # u_r at theta=180 = -disp_x at (-1, 0, 0)
  # u_r at theta=270 = -disp_y at (0, -1, 0)
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
  exodus = true
[]
