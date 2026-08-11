# 90-degree wedge rotated 30 degrees: cut faces at theta=30 and theta=120,
# both inclined (neither Cartesian-aligned). Exercises the constraint
# alongside two PenaltyInclinedNoDisplacementBC actions; matches Lame to
# within ~2 percent at the chosen penalty/E ratio.
[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  [annulus]
    type = AnnularMeshGenerator
    nr = 8
    nt = 16
    rmin = 0.5
    rmax = 1.0
    dmin = 30
    dmax = 120
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
  [InclinedNoDisplacementBC]
    [sym_theta30]
      boundary = dmin
      penalty = 1.0e10
    []
    [sym_theta120]
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
  [u_x_mid]
    type = PointValue
    variable = disp_x
    point = '0.2588190451 0.9659258263 0'
  []
  [u_y_mid]
    type = PointValue
    variable = disp_y
    point = '0.2588190451 0.9659258263 0'
  []
  [u_r_mid]
    type = LinearCombinationPostprocessor
    pp_names = 'u_x_mid u_y_mid'
    pp_coefs = '0.2588190451 0.9659258263'
  []
[]

[Outputs]
  exodus = true
[]
