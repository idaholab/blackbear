# Test of creep response due to stress ramp
# see figure 3.6 of:
# Munday, Lynn Brendon, et al. Multiscale-Informed Modeling of High Temperature
# Component Response with Uncertainty Quantification. No. INL/EXT-20-59795-Rev000.
# Idaho National Lab.(INL), Idaho Falls, ID (United States), 2020.

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 1
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[AuxVariables]
  [temperature]
    initial_condition = 889
  []
  [effective_inelastic_strain]
    order = FIRST
    family = MONOMIAL
  []
  [cell_dislocations]
    order = FIRST
    family = MONOMIAL
  []
  [wall_dislocations]
    order = FIRST
    family = MONOMIAL
  []
[]


[AuxKernels]
  [effective_inelastic_strain]
    type = ADMaterialRealAux
    variable = effective_inelastic_strain
    property = effective_creep_strain
  []
  [cell_dislocations]
    type = ADMaterialRealAux
    variable = cell_dislocations
    property = cell_dislocations
  []
  [wall_dislocations]
    type = ADMaterialRealAux
    variable = wall_dislocations
    property = wall_dislocations
  []
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
  [./symmy]
    type = ADDirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  [../]
  [./symmx]
    type = ADDirichletBC
    variable = disp_x
    boundary = left
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
    constant = -0.5
    function = shear_function
  [../]
  [./pressure_y]
    type = ADPressure
    variable = disp_y
    component = 1
    boundary = top
    constant = -0.5
    function = shear_function
  [../]
  [./pressure_z]
    type = ADPressure
    variable = disp_z
    component = 2
    boundary = front
    constant = 0.5
    function = shear_function
  [../]
[]

[Functions]
  [shear_function]
    type = ParsedFunction
    value = 'timeToDoubleInHours := 10;
            if(t<=28*60*60, 30.9e6, 30.9e6*(t-28*3600)/3600/timeToDoubleInHours+30.9e6)'
  []
[]

[Materials]
  [./elasticity_tensor]
    type = ADComputeIsotropicElasticityTensor
    youngs_modulus = 1.68e11
    poissons_ratio = 0.31
  [../]
  [./stress]
    type = ADComputeMultipleInelasticStress
    inelastic_models = rom_stress_prediction
  [../]
  [./mx_phase_fraction]
    type = ADGenericConstantMaterial
    prop_names = mx_phase_fraction
    prop_values = 5.13e-2  #precipitation bounds: 6e-3, 1e-1
    outputs = all
  [../]
  [./rom_stress_prediction]
     type = P91LAROMANCEStressUpdate
     temperature = temperature
     stress_input_window_failure = WARN
     cell_input_window_failure = ERROR
     wall_input_window_failure = ERROR
     temperature_input_window_failure = ERROR
     environment_input_window_failure = ERROR #This is the phase fraction, below
     environmental_factor = mx_phase_fraction
     initial_cell_dislocation_density = 2.3e12 #bounds 1e12 to 6e12
     initial_wall_dislocation_density = 1.41e13 # bounds 6e12 1.8e13
    #verbose = true
  [../]
[]


[Executioner]
  type = Transient

  solve_type = 'NEWTON'

  nl_abs_tol = 1e-12
  nl_rel_tol = 1e-4
  automatic_scaling = true
  compute_scaling_once = false

  dtmin = 0.1
  dtmax = 1e5
  end_time = 136800
  [TimeStepper]
    type = IterationAdaptiveDT
    dt = 0.1 ## This model requires a tiny timestep at the onset for the first 10s
    iteration_window = 4
    optimal_iterations = 12
    time_t = '100800'
    time_dt = '1e5'
  []

[]

[Postprocessors]
  [./effective_strain_avg]
    type = ElementAverageValue
    variable = effective_inelastic_strain
  [../]
  [./temperature]
    type = ElementAverageValue
    variable = temperature
  [../]
  [./cell_dislocations]
    type = ElementAverageValue
    variable = cell_dislocations
  [../]
  [./wall_disloactions]
    type = ElementAverageValue
    variable = wall_dislocations
  [../]
  [max_vonmises_stress]
    type = ElementExtremeValue
    variable = vonmises_stress
    value_type = max
  []
[]

[Outputs]
  csv = true
[]
