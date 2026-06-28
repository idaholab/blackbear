[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  [annulus]
    type = AnnularMeshGenerator
    nr = 2
    nt = 2
    rmin = 0.5
    rmax = 1.0
    dmin = 0
    dmax = 90
  []
  # Rotate so that the original xy-annulus lies in the yz-plane,
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
    num_layers = 2
    bottom_sideset = 'x_min'
    top_sideset = 'x_max'
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
  # Cut-face symmetry BCs in the yz-plane; the original dmin/dmax sidesets
  # are now the two cut faces of the wedge after rotation.
  [sym_dmin]
    type = DirichletBC
    variable = disp_z
    boundary = dmin
    value = 0
  []
  [sym_dmax]
    type = DirichletBC
    variable = disp_y
    boundary = dmax
    value = 0
  []
  [sym_xmin]
    type = DirichletBC
    variable = disp_x
    boundary = x_min
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

[Outputs]
  exodus = false
[]
