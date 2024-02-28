[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  [mesh]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 1
    ny = 1
    nz = 1
  []
[]

[AuxVariables]
  [./T]
    order = FIRST
    family = LAGRANGE
    initial_condition = 20.0
  [../]

  [./ASR_ex]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]

  [./ASR_vstrain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ASR_strain_xx]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./ASR_strain_yy]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./ASR_strain_zz]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./ASR_strain_xy]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./ASR_strain_yz]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./ASR_strain_zx]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./volumetric_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Physics/SolidMechanics/QuasiStatic]
  [./concrete]
    block = 0
    strain = SMALL
    add_variables = true
    eigenstrain_names = 'asr_expansion'
    generate_output = 'stress_xx stress_yy stress_zz stress_xy stress_yz stress_zx vonmises_stress hydrostatic_stress elastic_strain_xx elastic_strain_yy elastic_strain_zz strain_xx strain_yy strain_zz '
  [../]
[]

[AuxKernels]
  [./ASR_ex]
    type = MaterialRealAux
    variable = ASR_ex
    block = 0
    property = ASR_extent
    execute_on = 'timestep_end'
  [../]
  [./ASR_vstrain]
    type = MaterialRealAux
    block = 0
    variable = ASR_vstrain
    property = ASR_volumetric_strain
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_xx]
    type = RankTwoAux
    block = 0
    rank_two_tensor = asr_expansion
    variable = ASR_strain_xx
    index_i = 0
    index_j = 0
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_yy]
    type = RankTwoAux
    block = 0
    rank_two_tensor = asr_expansion
    variable = ASR_strain_yy
    index_i = 1
    index_j = 1
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_zz]
    type = RankTwoAux
    block = 0
    rank_two_tensor = asr_expansion
    variable = ASR_strain_zz
    index_i = 2
    index_j = 2
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_xy]
    type = RankTwoAux
    block = 0
    rank_two_tensor = asr_expansion
    variable = ASR_strain_xy
    index_i = 0
    index_j = 1
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_yz]
    type = RankTwoAux
    block = 0
    rank_two_tensor = asr_expansion
    variable = ASR_strain_yz
    index_i = 1
    index_j = 2
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_zx]
    type = RankTwoAux
    block = 0
    rank_two_tensor = asr_expansion
    variable = ASR_strain_zx
    index_i = 0
    index_j = 2
    execute_on = 'timestep_end'
  [../]

  [./volumetric_strain]
    type = RankTwoScalarAux
    scalar_type = VolumetricStrain
    rank_two_tensor = total_strain
    variable = volumetric_strain
  [../]
[]

[Functions]
  [./strain_function]
    type = ParsedFunction
    expression = 1.24e-3*(1-exp(-t/86400/8.68))/(1+exp((8.68-t/86400)/16.22))
  [../]
[]

[Materials]
  [elasticity_concrete]
    type = ComputeIsotropicElasticityTensor
    block = 0
    youngs_modulus = 37.3e9
    poissons_ratio = 0.22
  []
  [stress]
    type = ComputeLinearElasticStress
    block = 0
  []

  [ASR_expansion]
    type = ConcreteASREigenstrain
    block = 0
    expansion_type = Isotropic

    reference_temperature  = 20.0
    temperature_unit = Celsius
    max_volumetric_expansion =  1.24e-3

    characteristic_time = 8.68
    latency_time =  16.22
    characteristic_activation_energy = 5400.0
    latency_activation_energy = 9400.0
    stress_latency_factor = 1.0

    compressive_strength = 31.0e6
    compressive_stress_exponent = 0.0

    tensile_strength = 3.2e6
    tensile_retention_factor = 1.0
    tensile_absorption_factor = 1.0

    ASR_dependent_tensile_strength = false
    residual_tensile_strength_fraction = 1.0

    temperature = T
    relative_humidity = 0.0
    rh_exponent = 0.0
    eigenstrain_name = asr_expansion
  []
[]


[BCs]
  [./left]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./bottom]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./back]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0.0
  [../]
[]

[Postprocessors]
  [./ASR_strain]
    type = ElementAverageValue
    variable = ASR_vstrain
    block = 'ANY_BLOCK_ID 0'
  [../]
  [ASR_ext]
    type = ElementAverageValue
    variable = ASR_ex
    block = 'ANY_BLOCK_ID 0'
  []
  [./vonmises]
    type = ElementAverageValue
    variable = vonmises_stress
  [../]
  [./vstrain]
    type = ElementAverageValue
    variable = volumetric_strain
  [../]
[]

[Executioner]
  type       = Transient
  solve_type = 'PJFNK'
  line_search = none
  petsc_options_iname = '-pc_type -ksp_gmres_restart'
  petsc_options_value = 'lu       101'

  dt = 100000
  end_time = 12960000
  l_max_its  = 50
  l_tol      = 1e-6
  nl_max_its = 10
  nl_rel_tol = 1e-9
  nl_abs_tol = 1e-10
[]

[Outputs]
  exodus         = true
  perf_graph     = true
  csv = true
  [./Console]
    type = Console
  [../]
[]
