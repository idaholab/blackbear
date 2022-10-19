[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 2
  ny = 2
  second_order = true
[]

[GlobalParams]
  displacements = 'disp_x disp_y'
  volumetric_locking_correction = false
[]

[AuxVariables]
  [local_damage]
    order = CONSTANT
    family = MONOMIAL
  []
  [omega]
    order = CONSTANT
    family = MONOMIAL
  []
  [combined_creep_strain_yy]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [local_damage]
    type = MaterialRealAux
    variable = local_damage
    property = local_damage
    execute_on = TIMESTEP_END
  []
  [omega]
    type = MaterialRealAux
    variable = omega
    property = omega
  []
  [combined_creep_strain_yy]
    type = RankTwoAux
    variable = combined_creep_strain_yy
    rank_two_tensor = combined_creep
    index_j = 1
    index_i = 1
    execute_on = timestep_end
  []
[]

[Functions]
  [./pull]
    type = PiecewiseLinear
    x = '0 10'
    y = '0 1e-3'
  [../]
[]

[Modules/TensorMechanics/Master]
  [./all]
    strain = FINITE
    add_variables = true
    use_finite_deform_jacobian = true
    generate_output = 'hydrostatic_stress'
  [../]
[]

[Materials]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1e10
    poissons_ratio = 0.3
  [../]

  [./elastic_strain]
    type = ComputeMultipleInelasticStress
    # tangent_operator = nonlinear
    damage_model = 'steel_local'
  [../]

  [steel_local]
    type = SteelCreepDamageOh
    epsilon_f = 0.07
    creep_law_exponent = 10
    use_displaced_mesh = true
    damage_index_name = local_damage
    reduction_damage_threshold = 0.5
    use_old_damage = true
  []

  [./creep_ten]
    type = PowerLawCreepStressUpdate
    coefficient = 10e-24
    n_exponent = 4
    activation_energy = 0
    base_name = creep_ten
  [../]
  [./creep_ten2]
    type = PowerLawCreepStressUpdate
    coefficient = 10e-24
    n_exponent = 4
    activation_energy = 0
    base_name = creep_ten2
  [../]
  [./creep_one]
    type = PowerLawCreepStressUpdate
    coefficient = 20e-24
    n_exponent = 4
    activation_energy = 0
    base_name = creep_one
  [../]
[]

[BCs]
  [./no_disp_x]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]

  [./no_disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]

  [./pull_disp_y]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = top
    function = pull
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

  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  line_search = 'none'
  nl_rel_tol = 1e-5

  num_steps = 5
  dt = 0.025
[]

[Postprocessors]
  [./max_disp_x]
    type = ElementExtremeValue
    variable = disp_x
  [../]
  [./max_disp_y]
    type = ElementExtremeValue
    variable = disp_y
  [../]
  [./max_hydro]
    type = ElementAverageValue
    variable = hydrostatic_stress
  [../]
  [./dt]
    type = TimestepSize
  [../]
  [./num_lin]
    type = NumLinearIterations
    outputs = console
  [../]
  [./num_nonlin]
    type = NumNonlinearIterations
    outputs = console
  [../]
  [damage_index]
    type = ElementExtremeValue
    variable = local_damage
  []
  [omega]
    type = ElementAverageValue
    variable = omega
  []
  [combined_creep_strain_yy]
    type = ElementAverageValue
    variable = combined_creep_strain_yy
  []
[]

[Outputs]
  csv = true
  perf_graph = true
[]
