[Mesh]
  type = GeneratedMesh
  dim = 3
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[AuxVariables]
  [./temperature]
  [../]
[]

[AuxKernels]
  [./temp_aux]
    type = FunctionAux
    variable = temperature
    function = temp_fcn
    execute_on = 'initial timestep_begin'
  [../]
[]

[Functions]
  [./cell_fcn]
    type = PiecewiseConstant
    data_file = p91_verification_data.csv
    x_index_in_file = 0
    y_index_in_file = 1
    format = columns
    xy_in_file_only = false
    direction = left
  [../]
  [./wall_fcn]
    type = PiecewiseConstant
    data_file = p91_verification_data.csv
    x_index_in_file = 0
    y_index_in_file = 2
    format = columns
    xy_in_file_only = false
    direction = left
  [../]
  [./vmJ2_fcn]
    type = PiecewiseConstant
    data_file = p91_verification_data.csv
    x_index_in_file = 0
    y_index_in_file = 3
    format = columns
    xy_in_file_only = false
    direction = left
    scale_factor = 1
  [../]
  [./evm_fcn]
    type = PiecewiseConstant
    data_file = p91_verification_data.csv
    x_index_in_file = 0
    y_index_in_file = 4
    format = columns
    xy_in_file_only = false
    direction = left
  [../]
  [./temp_fcn]
    type = PiecewiseConstant
    data_file = p91_verification_data.csv
    x_index_in_file = 0
    y_index_in_file = 5
    format = columns
    xy_in_file_only = false
    # direction = left
  [../]
  [./phase_fraction_fcn]
    type = PiecewiseConstant
    data_file = p91_verification_data.csv
    x_index_in_file = 0
    y_index_in_file = 6
    format = columns
    xy_in_file_only = false
    direction = left
  [../]

  [./cell_rate_soln_fcn]
    type = PiecewiseConstant
    data_file = p91_verification_data.csv
    x_index_in_file = 0
    y_index_in_file = 8
    format = columns
    xy_in_file_only = false
    direction = left
  [../]
  [./wall_rate_soln_fcn]
    type = PiecewiseConstant
    data_file = p91_verification_data.csv
    x_index_in_file = 0
    y_index_in_file = 9
    format = columns
    xy_in_file_only = false
    direction = left
  [../]
  [./creep_rate_soln_fcn]
    type = PiecewiseConstant
    data_file = p91_verification_data.csv
    x_index_in_file = 0
    y_index_in_file = 10
    format = columns
    xy_in_file_only = false
    direction = left
  [../]

  [./cell_rate_diff_fcn]
    type = ParsedFunction
    vars = 'cell_rate_soln cell_rate'
    vals = 'cell_rate_soln cell_rate'
    value = 'abs((cell_rate_soln - cell_rate) / cell_rate_soln)'
  [../]
  [./wall_rate_diff_fcn]
    type = ParsedFunction
    vars = 'wall_rate_soln wall_rate'
    vals = 'wall_rate_soln wall_rate'
    value = 'abs((wall_rate_soln - wall_rate) / wall_rate_soln)'
  [../]
  [./creep_rate_diff_fcn]
    type = ParsedFunction
    vars = 'creep_rate_soln creep_rate'
    vals = 'creep_rate_soln creep_rate'
    value = 'abs((creep_rate_soln - creep_rate) / creep_rate_soln)'
  [../]

  [./filtered_cell_rate_diff_fcn]
    type = ParsedFunction
    vars = 'cell_rate_soln cell_rate wall_rate creep_rate'
    vals = 'cell_rate_soln cell_rate wall_rate creep_rate'
    value = 'cell_check := if(abs(cell_rate) > 1e12, 0, if(abs(cell_rate) < 100, 0, 1));
             wall_check := if(abs(wall_rate) > 1e12, 0, if(abs(wall_rate) < 100, 0, 1));
             creep_check := if(abs(creep_rate) > 1e-4, 0, if(abs(creep_rate) < 1e-13, 0, 1));
             abs((cell_rate_soln - cell_rate) / cell_rate_soln)*cell_check*wall_check*creep_check'
  [../]
  [./filtered_wall_rate_diff_fcn]
    type = ParsedFunction
    vars = 'wall_rate_soln wall_rate cell_rate creep_rate'
    vals = 'wall_rate_soln wall_rate cell_rate creep_rate'
    value = 'cell_check := if(abs(cell_rate) > 1e12, 0, if(abs(cell_rate) < 100, 0, 1));
             wall_check := if(abs(wall_rate) > 1e12, 0, if(abs(wall_rate) < 100, 0, 1));
             creep_check := if(abs(creep_rate) > 1e-4, 0, if(abs(creep_rate) < 1e-13, 0, 1));
             abs((wall_rate_soln - wall_rate) / wall_rate_soln)*cell_check*wall_check*creep_check'
  [../]
  [./filtered_creep_rate_diff_fcn]
    type = ParsedFunction
    vars = 'creep_rate_soln creep_rate wall_rate cell_rate'
    vals = 'creep_rate_soln creep_rate wall_rate cell_rate'
    value = 'cell_check := if(abs(cell_rate) > 1e12, 0, if(abs(cell_rate) < 100, 0, 1));
             wall_check := if(abs(wall_rate) > 1e12, 0, if(abs(wall_rate) < 100, 0, 1));
             creep_check := if(abs(creep_rate) > 1e-4, 0, if(abs(creep_rate) < 1e-13, 0, 1));
             abs((creep_rate_soln - creep_rate) / creep_rate_soln)*cell_check*wall_check*creep_check'
  [../]
[]


[Modules/TensorMechanics/Master]
  [./all]
    strain = FINITE
    add_variables = true
    generate_output = 'vonmises_stress'
    use_automatic_differentiation = true
  [../]
[]

[BCs]
  [./symmx]
    type = ADDirichletBC
    variable = disp_x
    boundary = left
    value = 0
  [../]
  [./symmy]
    type = ADDirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  [../]
  [./symmz]
    type = ADDirichletBC
    variable = disp_z
    boundary = back
    value = 0
  [../]
  [./pressure_x]
    type = ADPressure
    variable = disp_x
    component = 0
    boundary = right
    function = vmJ2_fcn
    constant = 0.5e6
  [../]
  [./pressure_y]
    type = ADPressure
    variable = disp_y
    component = 1
    boundary = top
    function = vmJ2_fcn
    constant = -0.5e6
  [../]
  [./pressure_z]
    type = ADPressure
    variable = disp_z
    component = 2
    boundary = front
    function = vmJ2_fcn
    constant = -0.5e6
  [../]
[]

[Materials]
  [./elasticity_tensor]
    type = ADComputeIsotropicElasticityTensor
    youngs_modulus = 1e11
    poissons_ratio = 0.3
  [../]
  [./stress]
    type = ADComputeMultipleInelasticStress
    inelastic_models = rom_stress_prediction
  [../]
  [./phase_fraction]
    type = ADGenericFunctionMaterial
    prop_names = 'phase_fraction'
    prop_values = 'phase_fraction_fcn'
    outputs = all
  [../]
  [./rom_stress_prediction]
    type = P91LAROMANCEStressUpdate
    temperature = temperature
    effective_inelastic_strain_name = effective_creep_strain
    internal_solve_full_iteration_history = true
    apply_strain = false
    outputs = all
    wall_dislocation_density_forcing_function = wall_fcn
    cell_dislocation_density_forcing_function = cell_fcn
    old_creep_strain_forcing_function = evm_fcn
    environmental_factor = phase_fraction
    #verbose = true
  [../]
[]

[Executioner]
  type = Transient

  solve_type = 'NEWTON'

  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  line_search = 'none'
  automatic_scaling = true

  nl_abs_tol = 1e-10

  dt = 0.1
  end_time = 400
[]

[Postprocessors]
  [./extrapolation]
    type = ElementAverageValue
    variable = ROM_extrapolation
    outputs = console
  [../]
  [./old_strain_in]
    type = FunctionValuePostprocessor
    function = evm_fcn
    execute_on = 'TIMESTEP_END initial'
    outputs = console
  [../]
  [./effective_strain_avg]
    type = ElementAverageValue
    variable = effective_creep_strain
    outputs = console
  [../]
  [./temperature]
    type = ElementAverageValue
    variable = temperature
    outputs = console
  [../]
  [./phase_fraction]
    type = ElementAverageValue
    variable = phase_fraction
    outputs = console
  [../]

  [./cell_rate]
    type = ElementAverageValue
    variable = cell_dislocation_rate
  [../]
  [./wall_rate]
    type = ElementAverageValue
    variable = wall_dislocation_rate
  [../]
  [./vonmises_stress]
    type = ElementAverageValue
    variable = vonmises_stress
    outputs = console
  [../]
  [./creep_rate]
    type = ElementAverageValue
    variable = creep_rate
  [../]

  [./cell_in]
    type = FunctionValuePostprocessor
    function = cell_fcn
    execute_on = 'TIMESTEP_END initial'
    outputs = console
  [../]
  [./wall_in]
    type = FunctionValuePostprocessor
    function = wall_fcn
    execute_on = 'TIMESTEP_END initial'
    outputs = console
  [../]
  [./vmJ2_in]
    type = FunctionValuePostprocessor
    function = vmJ2_fcn
    execute_on = 'TIMESTEP_END initial'
    outputs = console
  [../]
  [./cell_rate_soln]
    type = FunctionValuePostprocessor
    function = cell_rate_soln_fcn
    outputs = console
  [../]
  [./wall_rate_soln]
    type = FunctionValuePostprocessor
    function = wall_rate_soln_fcn
    outputs = console
  [../]
  [./creep_rate_soln]
    type = FunctionValuePostprocessor
    function = creep_rate_soln_fcn
    outputs = console
  [../]

  [./filtered_cell_rate_diff]
    type = FunctionValuePostprocessor
    function = filtered_cell_rate_diff_fcn
    outputs = console
  [../]
  [./filtered_wall_rate_diff]
    type = FunctionValuePostprocessor
    function = filtered_wall_rate_diff_fcn
    outputs = console
  [../]
  [./filtered_creep_rate_diff]
    type = FunctionValuePostprocessor
    function = filtered_creep_rate_diff_fcn
    outputs = console
  [../]

  [./zfiltered_cell_rate_max_diff]
    type = TimeExtremeValue
    postprocessor = filtered_cell_rate_diff
    outputs = console
  [../]
  [./zfiltered_wall_rate_max_diff]
    type = TimeExtremeValue
    postprocessor = filtered_wall_rate_diff
    outputs = console
  [../]
  [./zfiltered_creep_rate_max_diff]
    type = TimeExtremeValue
    postprocessor = filtered_creep_rate_diff
    outputs = console
  [../]
[]

[Outputs]
  interval = 1
  csv = true
[]
