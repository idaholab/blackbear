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

[AuxVariables]
  [./temperature]
    initial_condition = 773.0
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

[Functions]
  [./pres_prof]
    type = PiecewiseLinear
    x = '0 10'
    y = '1.2 7'
  [../]
  [./temp_func]
    type = ParsedFunction
    value = (773+t)*exp((100-abs(y))^10/2.0e22)
  [../]
[]

[Modules/TensorMechanics/Master]
  [./all]
    strain = FINITE
    add_variables = true
    generate_output = 'strain_xx strain_yy strain_zz strain_xy stress_xx stress_yy stress_zz stress_xy vonmises_stress'
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
    initial_mobile_dislocation_density = 8.0e12
    initial_immobile_dislocation_density = 7.0e11
  [../]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1.80e11 #from Takahashi et al (2006) Fusion Engineering and Design 81(1-7), Table 2
    poissons_ratio = 0.3 #from Takahashi et al (2006) Fusion Engineering and Design 81(1-7), Table 2
  [../]
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
    boundary = 1
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

# [Preconditioning]
#   [./SMP]
#     type = SMP
#     full=true
#   [../]
# []

[Executioner]
  type = Transient
  # solve_type = 'PJFNK'
  #
  # petsc_options_iname = '-pc_type -pc_asm_overlap -sub_pc_type -ksp_type -ksp_gmres_restart'
  # petsc_options_value = ' asm      2              lu            gmres     200'

  solve_type = 'NEWTON'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu superlu_dist'
  l_tol = 1e-3
  nl_abs_tol = 1e-6
  nl_rel_tol = 5e-5
  nl_max_its = 10
  l_max_its = 10
  line_search = none
  start_time = 0.0
  end_time = 1000 #50

  # [./TimeStepper]
  #   type = FunctionDT
  #   time_t = '0 10 250 400   1100'
  #   time_dt = '0.1 1.0 1.0 1.0  1.0'
  #   # min_dt = '0.00001'
  # [../]

  [./Predictor]
    type = SimplePredictor
    scale = 1
  [../]
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
  [./strain_xy]
    type = ElementAverageValue
    variable = strain_xy
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
  [./stress_xy]
    type = ElementAverageValue
    variable = stress_xy
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
  [./vonmises_stress]
    type = ElementAverageValue
    variable = vonmises_stress
  [../]
  [./effective_inelastic_strain]
    type = ElementAverageValue
    variable = effective_inelastic_strain
  [../]
[]


[Outputs]
  file_base = ROM_tube
  csv = true
  exodus = true
  print_linear_residuals = true
  perf_graph = true
[]
