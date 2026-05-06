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
  # Rotate so the original xy-annulus lies in the yz-plane,
  # making +x the cylindrical axis after extrusion.
  [rotate_to_yz]
    type = TransformGenerator
    input = annulus
    transform = ROTATE_WITH_MATRIX
    rotation_matrix = '0  0  1
                       0  1  0
                       -1 0  0'
  []
  [extrude]
    type = MeshExtruderGenerator
    input = rotate_to_yz
    extrusion_vector = '1.0 0 0'
    num_layers = 4
    bottom_sideset = 'x_min'
    top_sideset = 'x_max'
  []
  # Pin disp_z at the constraint primary node (0, 1, 0) on the x_min
  # face to break the residual rigid-body rotation about the x-axis that
  # the cylindrical constraint cannot suppress on its own.
  [pin_primary]
    type = ExtraNodesetGenerator
    input = extrude
    new_boundary = 'primary_pin'
    coord = '0 1 0'
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
  [sym_x_min]
    type = DirichletBC
    variable = disp_x
    boundary = x_min
    value = 0
  []
  [pin_primary_disp_z]
    type = DirichletBC
    variable = disp_z
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
    variable = disp_y
    displacements = 'disp_x disp_y disp_z'
    secondary = rmax
    cylindrical_axis = x
    penalty = 1.0e10
    primary_node_coord = '0 1 0'
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
  # Three angular probes around the (y, z) circle at outer radius:
  # (0, 1, 0) -- radial = +y, so u_r = +disp_y
  # (0, 0, 1) -- radial = +z, so u_r = +disp_z
  # (0, -1, 0) -- radial = -y, so u_r = -disp_y; this PP samples disp_y
  # and is expected to equal -u_r_theta0 under axisymmetric loading.
  [u_r_theta0]
    type = PointValue
    variable = disp_y
    point = '0 1.0 0'
  []
  [u_r_theta90]
    type = PointValue
    variable = disp_z
    point = '0 0 1.0'
  []
  [u_r_theta180]
    type = PointValue
    variable = disp_y
    point = '0 -1.0 0'
  []
[]

[Outputs]
  exodus = true
[]
