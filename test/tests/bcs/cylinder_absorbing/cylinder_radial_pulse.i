# Cylinder radial-pulse absorbing BC test
#
# A quarter-annular 3D mesh (1/4 cylinder, rmin=1 m, rmax=5 m, thin in z).
# A compact pressure pulse is applied on the inner surface (rmin); the outer
# surface (rmax) is terminated with CylinderAbsorbingBC.  The gold Exodus
# captures the radially outward displacement at rmax decaying after pulse
# passage -- a stress-free BC at rmax would produce persistent ringing instead.
#
# Material: rho = 2000 kg/m^3, lambda = mu = 1.6e9 Pa
#   c_p = sqrt((lambda + 2*mu)/rho) = sqrt(4.8e9/2000) ~ 1549 m/s
# Wave travel time: (rmax - rmin) / c_p = 4 / 1549 ~ 2.58e-3 s
# Comparison end time: 7e-3 s  (pulse fully past rmax; absorption vs ringing
# is visible in the 4.4e-3 to 7e-3 s window)

[Mesh]
  [annular]
    type = AnnularMeshGenerator
    nr = 12
    nt = 12
    rmin = 1.0
    rmax = 5.0
    dmin = 0.0
    dmax = 90.0
  []
  [extrude]
    type = MeshExtruderGenerator
    input = annular
    extrusion_vector = '0 0 0.5'
    bottom_sideset = 'zmin'
    top_sideset = 'zmax'
    num_layers = 1
  []
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Physics/SolidMechanics/Dynamic]
  [all]
    add_variables = true
    newmark_beta = 0.25
    newmark_gamma = 0.5
    strain = SMALL
    density = density
    generate_output = 'stress_xx stress_yy'
  []
[]

[BCs]
  # Plane-symmetry cuts: dmin is the y=0 plane (angle=0), dmax is the x=0 plane (angle=90)
  [sym_dmin_y]
    type = DirichletBC
    variable = disp_y
    boundary = dmin
    value = 0.0
  []
  [sym_dmax_x]
    type = DirichletBC
    variable = disp_x
    boundary = dmax
    value = 0.0
  []

  # Z faces -- thin slab: pin out-of-plane
  [fix_z_bottom]
    type = DirichletBC
    variable = disp_z
    boundary = zmin
    value = 0.0
  []
  [fix_z_top]
    type = DirichletBC
    variable = disp_z
    boundary = zmax
    value = 0.0
  []

  # Internal pressure pulse at the inner surface
  [pressure_inner]
    type = Pressure
    variable = disp_x
    boundary = rmin
    function = pressure_pulse
    displacements = 'disp_x disp_y disp_z'
  []

  # Absorbing BC at the outer surface -- one instance per displacement component
  [absorb_x_outer]
    type = CylinderAbsorbingBC
    variable = disp_x
    boundary = rmax
    displacements = 'disp_x disp_y disp_z'
    density = density
    shear_modulus = shear_modulus
    lambda = lambda
    component = x
  []
  [absorb_y_outer]
    type = CylinderAbsorbingBC
    variable = disp_y
    boundary = rmax
    displacements = 'disp_x disp_y disp_z'
    density = density
    shear_modulus = shear_modulus
    lambda = lambda
    component = y
  []
  [absorb_z_outer]
    type = CylinderAbsorbingBC
    variable = disp_z
    boundary = rmax
    displacements = 'disp_x disp_y disp_z'
    density = density
    shear_modulus = shear_modulus
    lambda = lambda
    component = z
  []
[]

[Functions]
  # Compact raised-cosine pressure pulse: 1 MPa peak, duration 2e-3 s
  [pressure_pulse]
    type = ParsedFunction
    expression = 'if(t < 2e-3, 0.5e6 * (1 - cos(2 * pi * t / 2e-3)), 0)'
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 4.0e9
    poissons_ratio = 0.25
  []
  [stress]
    type = ComputeLinearElasticStress
  []
  [density_mat]
    type = GenericConstantMaterial
    prop_names = 'density'
    prop_values = '2000.0'
  []
  # Lame parameters for the BC (E=4e9, nu=0.25 -> lambda = mu = 1.6e9 Pa)
  [lame_params]
    type = GenericConstantMaterial
    prop_names = 'shear_modulus lambda'
    prop_values = '1.6e9 1.6e9'
  []
[]

[Postprocessors]
  # Track peak radial displacement at the outer surface to verify decay vs ringing
  [disp_x_rmax_max]
    type = NodalExtremeValue
    variable = disp_x
    boundary = rmax
    value_type = max
  []
  [disp_y_rmax_max]
    type = NodalExtremeValue
    variable = disp_y
    boundary = rmax
    value_type = max
  []
[]

[Executioner]
  type = Transient
  start_time = 0.0
  end_time = 0.0035
  dt = 5.0e-5
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-14
  l_tol = 1e-8
[]

[Outputs]
  [out]
    type = Exodus
    time_step_interval = 5
  []
[]
