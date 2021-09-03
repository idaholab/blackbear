[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 3
  ny = 1
  nz = 1
  elem_type = HEX8
[]

[AuxVariables]
  [stress_xx]
    order = CONSTANT
    family = MONOMIAL
  []
  [strain_xx]
    order = CONSTANT
    family = MONOMIAL
  []
  [creep_strain_xx]
    order = CONSTANT
    family = MONOMIAL
  []
  [damage_index]
    order = CONSTANT
    family = MONOMIAL
  []
  [omega]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Modules/TensorMechanics/Master]
  [all]
    strain = FINITE
    incremental = true
    add_variables = true
    use_automatic_differentiation = true
  []
[]

[AuxKernels]
  [stress_xx]
    type = ADRankTwoAux
    variable = stress_xx
    rank_two_tensor = stress
    index_j = 0
    index_i = 0
    execute_on = timestep_end
  []
  [strain_xx]
    type = ADRankTwoAux
    variable = strain_xx
    rank_two_tensor = total_strain
    index_j = 0
    index_i = 0
    execute_on = timestep_end
  []
  [creep_strain_xx]
    type = ADRankTwoAux
    variable = creep_strain_xx
    rank_two_tensor = creep_strain
    index_j = 0
    index_i = 0
    execute_on = timestep_end
  []
  [damage_index]
    type = ADMaterialRealAux
    variable = damage_index
    property = damage_index
    execute_on = timestep_end
  []
  [omega]
    type = ADMaterialRealAux
    variable = omega
    property = omega
    execute_on = timestep_end
  []
[]

[BCs]
  [symmy]
    type = ADDirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  []
  [symmx]
    type = ADDirichletBC
    variable = disp_x
    boundary = left
    value = 0
  []
  [symmz]
    type = ADDirichletBC
    variable = disp_z
    boundary = back
    value = 0
  []
  [axial_load]
    type = ADFunctionDirichletBC
    variable = disp_x
    boundary = right
    function = pull
  []
[]

[Functions]
  [pull]
    type = ParsedFunction
    value = '5*t'
  []
[]

[Materials]
  [damage]
    type = ADSteelCreepDamageOh
    epsilon_f = 0.01
    creep_strain_name = creep_strain
    reduction_factor = 1.0e3
    use_old_damage = true
    creep_law_exponent = 10.0
  []

  [radial_return_stress]
    type = ADComputeMultipleInelasticStress
    inelastic_models = 'powerlawcrp'
    damage_model = damage
  []
  [powerlawcrp]
    type = ADPowerLawCreepStressUpdate
    coefficient = 3.125e-21
    n_exponent = 4.0
    m_exponent = 0.0
    activation_energy = 0.0
  []
  [elasticity]
    type = ADComputeIsotropicElasticityTensor
    poissons_ratio = 0.2
    youngs_modulus = 10e9
  []
[]

[Postprocessors]
  [stress_xx]
    type = ElementAverageValue
    variable = stress_xx
  []
  [strain_xx]
    type = ElementAverageValue
    variable = strain_xx
  []
  [creep_strain_xx]
    type = ElementAverageValue
    variable = creep_strain_xx
  []
  [damage_index]
    type = ElementExtremeValue
    variable = damage_index
  []
  [omega]
    type = ElementAverageValue
    variable = omega
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient

  l_max_its = 50
  l_tol = 1e-8
  nl_max_its = 20
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-8

  dt = 0.005
  dtmin = 0.005
  end_time = 0.65
[]

[Outputs]
  exodus = true
  csv = true
[]
