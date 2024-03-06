# Tests the ConcreteLogarithmicCreepModel in drying conditions
#
# Expected value for strain_xx
# 0.001 * (1 + 1.02 * (1 - exp(-t)) + 0.01 * (101 * log(1 + t)) + 0.025 * t)
#

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 1
  elem_type = HEX8
[]

[Functions]
  [./humidity_function]
    type = PiecewiseLinear
    x = '0 100 1000'
    y = '1 0 0'
  [../]
[]

[AuxVariables]
  [./h]
    order = CONSTANT
    family = MONOMIAL
    initial_condition = 1
  [../]
  [./creep_strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Physics]
  [SolidMechanics]
    [QuasiStatic]
      [all]
        strain = SMALL
        add_variables = true
        generate_output = 'stress_xx strain_xx'
      []
    []
  []
[]

[AuxKernels]
  [./humidity]
    type = FunctionAux
    variable = h
    function = humidity_function
    execute_on = timestep_begin
  [../]
  [./creep_strain_xx]
    type = RankTwoAux
    variable = creep_strain_xx
    rank_two_tensor = creep_strain
    index_j = 0
    index_i = 0
    execute_on = timestep_end
  [../]
[]

[BCs]
  [./symmy]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  [../]
  [./symmx]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0
  [../]
  [./symmz]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0
  [../]
  [./axial_load]
    type = NeumannBC
    variable = disp_x
    boundary = right
    value    = 10e6
  [../]
[]

[Materials]
  [./stress]
    type = ComputeLinearViscoelasticStress
  [../]
  [./logcreep]
    type = ConcreteLogarithmicCreepModel
    poissons_ratio = 0.2
    youngs_modulus = 10e9
    recoverable_youngs_modulus = 10e9
    recoverable_viscosity = 1
    long_term_viscosity = 1
    long_term_characteristic_time = 1
    humidity = h
    drying_creep_viscosity = 0.25
  [../]
[]

[UserObjects]
  [./visco_update]
    type = LinearViscoelasticityManager
    viscoelastic_model = logcreep
  [../]
[]

[Postprocessors]
  [./stress_xx]
    type = ElementAverageValue
    variable = stress_xx
    block = 'ANY_BLOCK_ID 0'
  [../]
  [./strain_xx]
    type = ElementAverageValue
    variable = strain_xx
    block = 'ANY_BLOCK_ID 0'
  [../]
  [./creep_strain_xx]
    type = ElementAverageValue
    variable = creep_strain_xx
    block = 'ANY_BLOCK_ID 0'
  [../]
  [./humidity]
    type = ElementAverageValue
    variable = h
    block = 'ANY_BLOCK_ID 0'
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient

  l_max_its  = 50
  l_tol      = 1e-8
  nl_max_its = 20
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-8

  dtmin = 0.01
  end_time = 100
  [./TimeStepper]
    type = LogConstantDT
    log_dt = 0.1
    first_dt = 0.1
  [../]

[]

[Outputs]
  file_base = concrete_logcreep_drying_out
  exodus = true
  csv = true
[]
