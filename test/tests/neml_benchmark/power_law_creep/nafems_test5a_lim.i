[GlobalParams]
  order = FIRST
  family = LAGRANGE
  volumetric_locking_correction = true
  displacements = 'disp_x disp_y'
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 1
  ny = 1
  xmax = 0.1
  ymax = 0.1
  elem_type = QUAD4
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
[]

[AuxVariables]
  [./creep]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Modules/TensorMechanics/Master]
  [./all]
    strain = FINITE
    add_variables = true
    generate_output = 'stress_xx stress_yy stress_zz stress_xy vonmises_stress hydrostatic_stress elastic_strain_xx elastic_strain_yy elastic_strain_zz creep_strain_xx creep_strain_yy creep_strain_zz strain_xx strain_yy strain_zz thirdinv_stress'
  [../]
[]

[AuxKernels]
  [./creep_aux]
    type = MaterialRealAux
    property = effective_creep_strain
    variable = creep
  [../]
[]

[BCs]
  [./bot_y]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./side_x]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./top_press]
    type = Pressure
    variable = disp_y
    boundary = top
    component = 1
    factor = -100.0
  [../]
  [./side_press]
    type = Pressure
    variable = disp_x
    boundary = right
    component = 0
    factor = -200.0
  [../]
[]

[Materials]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    block = 0
    youngs_modulus = 200e3
    poissons_ratio = 0.3
  [../]
  [./radial_return_stress]
    type = ComputeMultipleInelasticStress
    block = 0
    inelastic_models = 'powerlawcrp'
  [../]
  [./powerlawcrp]
    type = PowerLawCreepStressUpdate
    block = 0
    coefficient = 3.125e-14
    n_exponent = 5.0
    m_exponent = 0.0
    activation_energy = 0.0
    max_inelastic_increment = 0.01
  [../]
[]

[Executioner]
  type = Transient

  solve_type = 'PJFNK'

  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = ' lu       superlu_dist'

  line_search = 'none'

  nl_rel_tol = 1e-5
  nl_abs_tol = 1e-8
  l_max_its = 50
  nl_max_its = 100
  end_time = 1000.0
  l_tol = 1e-3

  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-6
    time_t = '1e-6  2e-6 3e-6 5e-6 9e-6 1.7e-5 3.3e-5 6.5e-5 1.29e-4 2.57e-4 5.13e-4 1.025e-3 2.049e-3 4.097e-3 8.193e-3 1.638e-2 3.276e-2 5.734e-2 0.106 0.180 0.291 0.457 0.706 1.08 1.64 2.48 3.74 5.63 8.46 12.7 19.1 28.7 43.0 64.5 108.0 194.0 366.0 710.0 1000.0'
    time_dt = '1e-6 1e-6 2e-6 4e-6 8e-6 1.6e-5 3.2e-5 6.4e-5 1.28e-4 2.56e-4 5.12e-4 1.024e-3 2.048e-3 4.096e-3 8.192e-3 1.6384e-2 2.458e-2 4.915e-2 7.40e-2 0.111 0.166 0.249 0.374 0.560 0.840 1.26 1.89 2.83 4.25 6.40 9.6 14.3 21.5 43.0 86.1 172.0 344.0 290.0 290.0'
    optimal_iterations = 30
    iteration_window = 9
    growth_factor = 2.0
    cutback_factor = 0.5
  [../]
[]

[Postprocessors]
  [./sigma_xx]
    type = ElementAverageValue
    variable = stress_xx
  [../]
  [./sigma_yy]
    type = ElementAverageValue
    variable = stress_yy
  [../]
  [./sigma_zz]
    type = ElementAverageValue
    variable = stress_zz
  [../]
  [./vonmises]
    type = ElementAverageValue
    variable = vonmises_stress
  [../]
  [./pressure]
    type = ElementAverageValue
    variable = hydrostatic_stress
  [../]
  [./invariant3]
    type = ElementAverageValue
    variable = thirdinv_stress
  [../]
  [./eps_crp_xx]
    type = ElementAverageValue
    variable = creep_strain_xx
  [../]
  [./eps_crp_yy]
    type = ElementAverageValue
    variable = creep_strain_yy
  [../]
  [./eps_crp_zz]
    type = ElementAverageValue
    variable = creep_strain_zz
  [../]
  [./eps_crp_mag]
    type = ElementAverageValue
    variable = creep
  [../]
  [./disp_x2]
    type = NodalVariableValue
    nodeid = 1
    variable = disp_x
  [../]
  [./disp_x3]
    type = NodalVariableValue
    nodeid = 2
    variable = disp_x
  [../]
  [./disp_y3]
    type = NodalVariableValue
    nodeid = 2
    variable = disp_y
  [../]
  [./disp_y4]
    type = NodalVariableValue
    nodeid = 3
    variable = disp_y
  [../]
  [./_dt]
    type = TimestepSize
  [../]
  [./elas_str_xx]
    type = ElementAverageValue
    variable = elastic_strain_xx
  [../]
  [./elas_str_yy]
    type = ElementAverageValue
    variable = elastic_strain_yy
  [../]
  [./elas_str_zz]
    type = ElementAverageValue
    variable = elastic_strain_zz
  [../]
#  [./num_lin_it]
#    type = NumLinearIterations
#  [../]
#  [./num_nonlin_it]
#    type = NumNonlinearIterations
#  [../]
#  [./tot_lin_it]
#    type = CumulativeValuePostprocessor
#    postprocessor = num_lin_it
#  [../]
#  [./tot_nonlin_it]
#    type = CumulativeValuePostprocessor
#    postprocessor = num_nonlin_it
#  [../]
#  [./alive_time]
#    type = PerfGraphData
#    section_name = Root
#    data_type = TOTAL
#  [../]
[]

[Outputs]
  print_linear_residuals = true
  perf_graph = true
  csv = true
  [./out]
    type = Exodus
    elemental_as_nodal = true
  [../]
  [./console]
    type = Console
    max_rows = 25
  [../]
[]
