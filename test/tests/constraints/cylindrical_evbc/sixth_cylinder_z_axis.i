[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  [annulus]
    type = AnnularMeshGenerator
    nr = 8
    nt = 8
    rmin = 0.5
    rmax = 1.0
    dmin = 0
    dmax = 60
  []
  [extrude]
    type = MeshExtruderGenerator
    input = annulus
    extrusion_vector = '0 0 1.0'
    num_layers = 4
    bottom_sideset = 'bottom'
    top_sideset = 'top'
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
  [sym_theta0]
    type = DirichletBC
    variable = disp_y
    boundary = dmin
    value = 0
  []
  [InclinedNoDisplacementBC]
    [sym_theta60]
      boundary = dmax
      penalty = 1.0e10
    []
  []
  [sym_bottom]
    type = DirichletBC
    variable = disp_z
    boundary = bottom
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
  # Probe at theta=30 (mid-arc) on outer surface; radial direction
  # is (cos30, sin30, 0). u_r = disp_x*cos30 + disp_y*sin30.
  [u_x_mid]
    type = PointValue
    variable = disp_x
    point = '0.8660254038 0.5 0'
  []
  [u_y_mid]
    type = PointValue
    variable = disp_y
    point = '0.8660254038 0.5 0'
  []
  [u_r_mid]
    type = LinearCombinationPostprocessor
    pp_names = 'u_x_mid u_y_mid'
    pp_coefs = '0.8660254038 0.5'
  []
[]

[Outputs]
  exodus = true
[]
