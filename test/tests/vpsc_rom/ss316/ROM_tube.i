[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  type = FileMesh
  file = cylinder_mesh2.e
  construct_side_list_from_node_list = true
[]

[Problem]
  coord_type = RZ
[]


[Modules/TensorMechanics/Master]
  [./all]
    strain = FINITE
    add_variables = true
    generate_output = 'strain_xx strain_yy strain_zz strain_yz strain_zx strain_xy stress_xx stress_yy stress_zz stress_yz stress_zx stress_xy vonmises_stress'
  [../]
[]

[AuxVariables]
  [./temperature]
  [../]
  [./burst]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./mobile_dislocations]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./immobile_dislocations]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./effective_inelastic_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./temperature]
    variable = temperature
    type = FunctionAux
    function = temp_func
  [../]
  [./burst]
    variable = burst
    type = ParsedAux
    args = 'stress_zz temperature'
    function = 'stress_zz-2648925.20204857*exp(-0.0091099171*temperature)'
  [../]
  [./mobile_dislocations]
    type = MaterialRealAux
    property = mobile_dislocations
    variable = mobile_dislocations
  [../]
  [./immobile_dislocations]
    type = MaterialRealAux
    property = immobile_dislocations
    variable = immobile_dislocations
  [../]
  [./effective_inelastic_strain]
    type = RankTwoScalarAux
    rank_two_tensor = creep_strain
    scalar_type = EffectiveStrain
    variable = effective_inelastic_strain
  [../]
[]

[Materials]
  [./stress]
    type = ComputeMultipleInelasticStress
    inelastic_models = rom_stress_prediction
  [../]
  [./rom_stress_prediction]
    type = SS316ROMStressPrediction
    temperature = temperature
    initial_mobile_dislocation_density = 1e13
    initial_immobile_dislocation_density = 1e12
  [../]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 3.30e11 #approximation from Clausen et al (2016) JNM 425(1-3), Table 2
    poissons_ratio = 0.3 #approximation from Clausen et al (2016) JNM 425(1-3), Table 2
  [../]
  # [./elasticity_tensor_anisotropic]
  #   type = ComputeElasticityTensor
  #   C_ijkl = '143.5e3 72.5e3 65.4e3 143.5e3 65.4e3 164.9e3 32.1e3 32.1e3 32.1e3'
  #   fill_method = symmetric9
  # [../]
[]


[BCs]
  [./bc11]
    type = PresetBC
    variable = disp_x
    boundary = 8
    value = 0.0
  [../]
  [./bc12]
    type = PresetBC
    variable = disp_y
    boundary = 8
    value = 0.0
  [../]
  [./bc4]
    type = PresetBC
    variable = disp_x
    boundary = 1
    value = 0.0
  [../]
  [./Pressure]
    [./pres1]
      disp_y = disp_y
      disp_x = disp_x
      factor = 1e6
      function = pres_prof
      boundary = '3 6 10'
    [../]
  [../]
[]

[Functions]
  [./rampConstant]
    type = PiecewiseLinear
    x = '0. 1. 2.'
    y = '1. 1. 1.'
    scale_factor = 1.0
  [../]
  [./pres_prof]
    type = PiecewiseLinear
    x = '0 10 1010'
    y = '0 14 14'
  [../]
  [./temp_func]
    # type = PiecewiseLinear
    # x = '0 10 1010'
    # y = '573 573 1573'
    type = ParsedFunction
    value = (873+t)*exp((100-abs(y))^10/2.0e22)
  [../]
  [./axial_prof]
    type = PiecewiseLinear
    x = '0 1 150 150.1 500'
    y = '0 11.83766 11.83766 50.31 50.31'
  [../]
  # [./burst_func]
  #   type = ParsedFunction
  #   # vars = 'a1 a2'
  #   # vals = '2648925.20204857 0.0091099171'
  #   value = 'stress_tt-2648925.20204857*exp(-0.0091099171*temperature)'
  # [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full=true
  [../]
[]

[Executioner]
  type = Transient

  solve_type = 'NEWTON'
  # solve_type = 'PJFNK'
  # solve_type = 'FD'


  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu superlu_dist'
  l_tol = 1e-3
  nl_abs_tol = 1e-4
  nl_rel_tol = 1e-6
  nl_max_its = 10
  l_max_its = 10
  line_search = none
  start_time = 0.0
  end_time = 50 #1000

  [./TimeStepper]
    type = FunctionDT
    time_t = '0 10 250 400   1100'
    time_dt = '0.1 1.0 1.0 1.0  1.0'
    # min_dt = '0.00001'
  [../]
  # [./Predictor] #Not linear loading, so don't use the simple predictor
  #   type = SimplePredictor
  #   scale = 1
  # [../]
[]

[Postprocessors]
  [./strain_xx]
    type = ElementAverageValue
    variable = strain_xx
  [../]
  [./strain_yy]
    type = ElementAverageValue
    variable = strain_yy
  [../]
  [./strain_zz]
    type = ElementAverageValue
    variable = strain_zz
  [../]
  [./strain_zx]
    type = ElementAverageValue
    variable = strain_zx
  [../]
  [./strain_xy]
    type = ElementAverageValue
    variable = strain_xy
  [../]
  [./strain_yz]
    type = ElementAverageValue
    variable = strain_yz
  [../]
  [./stress_zz]
    type = ElementAverageValue
    variable = stress_zz
  [../]
  [./stress_xx]
    type = ElementAverageValue
    variable = stress_xx
  [../]
  [./stress_yy]
    type = ElementAverageValue
    variable = stress_yy
  [../]
  [./stress_zx]
    type = ElementAverageValue
    variable = stress_zx
  [../]
  [./stress_xy]
    type = ElementAverageValue
    variable = stress_xy
  [../]
  [./stress_yz]
    type = ElementAverageValue
    variable = stress_yz
  [../]
  [./temperature]
    type = ElementAverageValue
    variable = temperature
  [../]
  [./burst]
    type = ElementAverageValue
    variable = burst
  [../]
  [./mobile_dislocations]
    type = ElementAverageValue
    variable = mobile_dislocations
  [../]
  [./immobile_dislocations]
    type = ElementAverageValue
    variable = immobile_dislocations
  [../]
  [./effective_inelastic_strain]
    type = ElementAverageValue
    variable = effective_inelastic_strain
  [../]
[]


[Outputs]
  file_base = ROM_tube
  output_initial = true
  csv = true
  print_linear_residuals = true
  perf_graph = true
  # output_on = 'initial timestep_end'
  # interval = 2
  [./exodus]
   type = Exodus
   # interval = 2
  [../]
[]
