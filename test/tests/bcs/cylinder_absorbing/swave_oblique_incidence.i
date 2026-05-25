# S-wave oblique-incidence absorbing BC test
#
# A Gaussian transverse (x-direction) displacement pulse is applied at the
# bottom of a 1D bar (thin 3D slab), creating a shear wave that travels in
# the +y direction.  The top surface is terminated with CylinderAbsorbingBC.
# The tangential-velocity term in the residual absorbs the shear wave; if that
# term were dropped the wave would reflect fully.
#
# Material: rho = 2000 kg/m^3, lambda = mu = 1.6e9 Pa
#   c_p = sqrt((lambda + 2*mu)/rho) = sqrt(4.8e9/2000) ~ 1549 m/s
#   c_s = sqrt(mu/rho)              = sqrt(1.6e9/2000)  ~  894 m/s
# Bar length L = 10 m  ->  2*L/c_s ~ 0.02238 s  (shear-pulse round-trip time)
# Pulse: Gaussian centered at t0 = L/c_s/2 ~ 0.005590 s, sigma = 5e-4 s

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 180
  nz = 1
  xmin = 0.0
  xmax = 0.5
  ymin = 0.0
  ymax = 10.0
  zmin = 0.0
  zmax = 0.5
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
    generate_output = 'stress_xy'
  []
[]

[BCs]
  # Fix lateral faces in the propagation direction and out-of-plane
  [fix_y_sides]
    type = DirichletBC
    variable = disp_y
    boundary = 'left right front back'
    value = 0.0
  []
  [fix_z_sides]
    type = DirichletBC
    variable = disp_z
    boundary = 'left right front back'
    value = 0.0
  []
  [fix_y_ends]
    type = DirichletBC
    variable = disp_y
    boundary = 'bottom top'
    value = 0.0
  []
  [fix_z_ends]
    type = DirichletBC
    variable = disp_z
    boundary = 'bottom top'
    value = 0.0
  []
  # Transverse (shear) Gaussian pulse applied at the bottom (y=0)
  [pulse_x_bottom]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = bottom
    function = gaussian_shear_pulse
  []

  # Absorbing BC at the top (y=L) -- one instance per displacement component
  [absorb_x_top]
    type = CylinderAbsorbingBC
    variable = disp_x
    boundary = top
    displacements = 'disp_x disp_y disp_z'
    density = density
    shear_modulus = shear_modulus
    lambda = lambda
    component = x
  []
  [absorb_y_top]
    type = CylinderAbsorbingBC
    variable = disp_y
    boundary = top
    displacements = 'disp_x disp_y disp_z'
    density = density
    shear_modulus = shear_modulus
    lambda = lambda
    component = y
  []
  [absorb_z_top]
    type = CylinderAbsorbingBC
    variable = disp_z
    boundary = top
    displacements = 'disp_x disp_y disp_z'
    density = density
    shear_modulus = shear_modulus
    lambda = lambda
    component = z
  []
[]

[Functions]
  # Gaussian shear pulse: peak = 1e-3 m, t0 = 5.59e-3 s, sigma = 5e-4 s
  [gaussian_shear_pulse]
    type = ParsedFunction
    expression = '1e-3 * exp(-0.5 * ((t - 5.59e-3) / 5e-4)^2)'
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
  # Maximum transverse displacement at the source face -- detects S-wave reflection.
  # Incident peak arrives near t ~ 5.6e-3 s.  Any secondary peak after
  # t ~ 16.8e-3 s (return trip via c_s) indicates the shear wave was reflected.
  [disp_x_bottom_max]
    type = NodalExtremeValue
    variable = disp_x
    boundary = bottom
    value_type = max
  []
  [disp_x_bottom_min]
    type = NodalExtremeValue
    variable = disp_x
    boundary = bottom
    value_type = min
  []

  # Interior probe at bar midpoint (y=L/2=5 m) -- free of Dirichlet/BC-face numerics.
  # Incident pulse passes through at t~11.18e-3 s; reflected pulse (if any) at t~22.37e-3 s.
  [disp_x_probe]
    type = PointValue
    variable = disp_x
    point = '0.25 5.0 0.25'
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [disp_x_abs]
    type = ParsedPostprocessor
    pp_names = 'disp_x_probe'
    expression = 'abs(disp_x_probe)'
    execute_on = 'INITIAL TIMESTEP_END'
  []

  # Gate to zero outside each window; TimeExtremeValue accumulates the running max.
  # Incidence window: pulse passes midpoint at t ~ 11.18e-3 s; window [8e-3, 14e-3]
  [incident_gated]
    type = ParsedPostprocessor
    expression = 'if(t >= 8e-3 & t <= 14e-3, disp_x_abs, 0)'
    pp_names = 'disp_x_abs'
    use_t = true
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [incident_peak]
    type = TimeExtremeValue
    postprocessor = incident_gated
    value_type = abs_max
    execute_on = 'INITIAL TIMESTEP_END'
  []

  # Reflection window: reflected pulse passes midpoint at t ~ 22.37e-3 s; window [20e-3, 26e-3]
  [reflected_gated]
    type = ParsedPostprocessor
    expression = 'if(t >= 20e-3 & t <= 26e-3, disp_x_abs, 0)'
    pp_names = 'disp_x_abs'
    use_t = true
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [reflected_peak]
    type = TimeExtremeValue
    postprocessor = reflected_gated
    value_type = abs_max
    execute_on = 'INITIAL TIMESTEP_END'
  []

  # Reflection ratio: should be < 0.05 for a working absorbing BC.
  # Guard against division by zero before the incident pulse arrives.
  [reflection_ratio]
    type = ParsedPostprocessor
    expression = 'if(incident_peak > 1e-12, reflected_peak / incident_peak, 0)'
    pp_names = 'reflected_peak incident_peak'
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  start_time = 0.0
  end_time = 0.027
  dt = 3.0e-5
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-14
  l_tol = 1e-8
[]

[Outputs]
  csv = true
[]
