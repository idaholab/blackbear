[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  # AnnularMeshGenerator closes the ring when dmin == 0 and dmax == 360
  # (full_annulus branch -- no duplicate seam nodes).
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
  # Pin disp_y at the constraint primary node (1, 0, 0) on the bottom
  # face to break the residual rigid-body rotation about the z-axis that
  # the cylindrical constraint cannot suppress on its own.
  [pin_primary]
    type = ExtraNodesetGenerator
    input = extrude
    new_boundary = 'primary_pin'
    coord = '1 0 0'
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
      factor = 1.5e7
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
  # At theta=180 the outward radial direction is (-1, 0, 0), so
  # u_r(theta=180) = -disp_x; this PP samples disp_x and is expected to
  # equal -u_r_theta0 under axisymmetric loading.
  [u_r_theta180]
    type = PointValue
    variable = disp_x
    point = '-1.0 0 0'
  []
[]

[Outputs]
  exodus = true
[]
