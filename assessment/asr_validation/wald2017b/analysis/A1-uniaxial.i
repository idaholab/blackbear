[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  penalty = 1e12
[]

[Problem]
  type = ReferenceResidualProblem
  reference_vector = 'ref'
  extra_tag_vectors = 'ref'
  group_variables = 'disp_x disp_y disp_z'
  acceptable_multiplier = 10
  acceptable_iterations = 50
[]

[Mesh]
  file = A1-uniaxial.e
[]

[Variables]
  [T]
    initial_condition = 10.6
  []
  [rh]
    initial_condition = 0.8
    block = 1
  []
  [disp_x]
  []
  [disp_y]
  []
  [disp_z]
  []
[]

[AuxVariables]
  [resid_x]
  []
  [resid_y]
  []
  [resid_z]
  []
  [ASR_ex]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [ASR_vstrain]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [ASR_strain_xx]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [ASR_strain_yy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [ASR_strain_zz]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [ASR_strain_xy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [ASR_strain_yz]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [ASR_strain_zx]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [volumetric_strain]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [thermal_strain_xx]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [thermal_strain_yy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [thermal_strain_zz]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [thermal_conductivity]
    order = CONSTANT
    family = Monomial
  []
  [thermal_capacity]
    order = CONSTANT
    family = Monomial
  []
  [humidity_diffusivity]
    order = CONSTANT
    family = Monomial
  []
  [asr_damage_index]
    order = CONSTANT
    family = MONOMIAL
  []
  [mazars_damage_index]
    order = CONSTANT
    family = MONOMIAL
  []
  [combined_damage_index]
    order = CONSTANT
    family = MONOMIAL
  []
  [area]
    order = CONSTANT
    family = MONOMIAL
  []
  [axial_stress]
    order = CONSTANT
    family = MONOMIAL
  []
  [axial_strain]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Modules/TensorMechanics/Master]
  [concrete]
    block = 1
    strain = FINITE
    eigenstrain_names = 'asr_expansion thermal_expansion'
    generate_output = 'stress_xx stress_yy stress_zz stress_xy stress_yz stress_zx vonmises_stress '
                      'hydrostatic_stress elastic_strain_xx elastic_strain_yy elastic_strain_zz '
                      'strain_xx strain_yy strain_zz'
    extra_vector_tags = 'ref'
    temperature = T
  []
[]

[Modules/TensorMechanics/LineElementMaster]
  [Reinforcement_block]
    block = '2'
    truss = true
    area = area
    #Note: Intentionally not including this here to have it give a nonzero
    #      displacement reference residual since it's an unrestrained problem
    #extra_vector_tags = 'ref'
  []
[]

[Constraints]
  [rebar_x2]
    type = EqualValueEmbeddedConstraint
    secondary = 2
    primary = 1
    variable = 'disp_x'
    primary_variable = 'disp_x'
    formulation = penalty
  []
  [rebar_y2]
    type = EqualValueEmbeddedConstraint
    secondary = 2
    primary = 1
    variable = 'disp_y'
    primary_variable = 'disp_y'
    formulation = penalty
  []
  [rebar_z2]
    type = EqualValueEmbeddedConstraint
    secondary = 2
    primary = 1
    variable = 'disp_z'
    primary_variable = 'disp_z'
    formulation = penalty
  []
  [rebar_T2]
    type = EqualValueEmbeddedConstraint
    secondary = 2
    primary = 1
    variable = 'T'
    primary_variable = 'T'
    formulation = penalty
    penalty = 1e6
  []
[]

[Kernels]
  [T_td]
    type = ConcreteThermalTimeIntegration
    variable = T
    block = 1
    extra_vector_tags = 'ref'
  []
  [T_diff]
    type = ConcreteThermalConduction
    variable = T
    block = 1
    extra_vector_tags = 'ref'
  []

  [T_conv]
    type = ConcreteThermalConvection
    variable = T
    relative_humidity = rh
    block = 1
    extra_vector_tags = 'ref'
  []

  [T_adsorption]
    type = ConcreteLatentHeat
    variable = T
    H = rh
    block = 1
    extra_vector_tags = 'ref'
  []

  [rh_td]
    type = ConcreteMoistureTimeIntegrationMassLumped
    variable = rh
    block = 1
    extra_vector_tags = 'ref'
  []

  [rh_diff]
    type = ConcreteMoistureDiffusion
    variable = rh
    temperature = T
    block = 1
    extra_vector_tags = 'ref'
  []
  [heat_dt]
    type = TrussHeatConductionTimeDerivative
    variable = T
    area = area
    block = 2
    extra_vector_tags = 'ref'
  []
  [heat_conduction]
    type = TrussHeatConduction
    variable = T
    area = area
    block = 2
    extra_vector_tags = 'ref'
  []
[]

[AuxKernels]
  [ASR_ex]
    type = MaterialRealAux
    variable = ASR_ex
    block = 1
    property = ASR_extent
    execute_on = 'timestep_end'
  []
  [ASR_vstrain]
    type = MaterialRealAux
    block = 1
    variable = ASR_vstrain
    property = ASR_volumetric_strain
    execute_on = 'timestep_end'
  []

  [ASR_strain_xx]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_xx
    index_i = 0
    index_j = 0
    execute_on = 'timestep_end'
  []
  [ASR_strain_yy]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_yy
    index_i = 1
    index_j = 1
    execute_on = 'timestep_end'
  []
  [ASR_strain_zz]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_zz
    index_i = 2
    index_j = 2
    execute_on = 'timestep_end'
  []

  [ASR_strain_xy]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_xy
    index_i = 0
    index_j = 1
    execute_on = 'timestep_end'
  []

  [ASR_strain_yz]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_yz
    index_i = 1
    index_j = 2
    execute_on = 'timestep_end'
  []

  [ASR_strain_zx]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_zx
    index_i = 0
    index_j = 2
    execute_on = 'timestep_end'
  []
  [thermal_strain_xx]
    type = RankTwoAux
    block = 1
    rank_two_tensor = thermal_expansion
    variable = thermal_strain_xx
    index_i = 0
    index_j = 0
    execute_on = 'timestep_end'
  []
  [thermal_strain_yy]
    type = RankTwoAux
    block = 1
    rank_two_tensor = thermal_expansion
    variable = thermal_strain_yy
    index_i = 1
    index_j = 1
    execute_on = 'timestep_end'
  []
  [thermal_strain_zz]
    type = RankTwoAux
    block = 1
    rank_two_tensor = thermal_expansion
    variable = thermal_strain_zz
    index_i = 2
    index_j = 2
    execute_on = 'timestep_end'
  []

  [volumetric_strain]
    type = RankTwoScalarAux
    scalar_type = VolumetricStrain
    rank_two_tensor = total_strain
    variable = volumetric_strain
    block = 1
  []

  [k]
    type = MaterialRealAux
    variable = thermal_conductivity
    property = thermal_conductivity
    execute_on = 'timestep_end'
    block = 1
  []
  [capacity]
    type = MaterialRealAux
    variable = thermal_capacity
    property = thermal_capacity
    execute_on = 'timestep_end'
    block = 1
  []

  [rh_duff]
    type = MaterialRealAux
    variable = humidity_diffusivity
    property = humidity_diffusivity
    execute_on = 'timestep_end'
    block = 1
  []

  [asr_damage_index]
    type = MaterialRealAux
    block = 1
    variable = asr_damage_index
    property = asr_damage_index
    execute_on = timestep_end
  []
  [mazars_damage_index]
    type = MaterialRealAux
    block = 1
    variable = mazars_damage_index
    property = mazars_damage_index
    execute_on = timestep_end
  []
  [combined_damage_index]
    type = MaterialRealAux
    block = 1
    variable = combined_damage_index
    property = combined_damage_index
    execute_on = timestep_end
  []
  [area]
    type = ConstantAux
    block = '2'
    variable = area
    value = 1.29e-4
    execute_on = 'initial timestep_begin'
  []
  [axial_stress]
    type = MaterialRealAux
    block = '2'
    variable = axial_stress
    property = axial_stress
  []
[]

[Functions]
  [ramp_temp]
    type = PiecewiseLinear
    data_file = temperature_history.csv
    format = columns
  []

  [ramp_humidity]
    type = PiecewiseLinear
    data_file = humidity_history.csv
    format = columns
  []
[]

[Materials]

  [concrete]
    type = ConcreteThermalMoisture
    block = 1
    # setup thermal property models and parameters
    # options available: CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004 KIM-2003
    thermal_model = EUROCODE-2004

    ref_density = 2231.0 # in kg/m^3
    ref_specific_heat = 1100.0 # in J/(Kg.0C)

    # setup moisture capacity and humidity diffusivity models
    aggregate_pore_type = dense #options: dense porous
    aggregate_mass = 1877.0 #mass of aggregate (kg) per m^3 of concrete
    aggregate_vol_fraction = 0.7
    cement_type = 1 #options: 1 2 3 4
    cement_mass = 354.0 #mass of cement (kg) per m^3 of concrete
    water_to_cement_ratio = 0.5
    concrete_cure_time = 28.0 #curing time in (days)

    # options available for humidity diffusivity:
    moisture_model = Xi #options: Bazant Mensi
    D1 = 3.0e-8

    coupled_moisture_diffusivity_factor = 1.0e-2 # factor for mositure diffusivity due to heat

    # coupled nonlinear variables
    relative_humidity = rh
    temperature = T
  []

  [creep]
    type = LinearViscoelasticStressUpdate
    block = 1
  []
  [logcreep]
    type = ConcreteLogarithmicCreepModel
    block = 1
    poissons_ratio = 0.22
    youngs_modulus = 37.3e9
    recoverable_youngs_modulus = 19e9 # scales up/down the maximum creep strain
    recoverable_viscosity = 2592000 # governs how fast the max creep strain is reached; 30 days
    long_term_viscosity = 138240000 # effect the time on the linear elastic portion; 4.38 years
    long_term_characteristic_time = 138240000 # effect how slow the saturation reaches; 4.38 years
    humidity = rh
    temperature = T
    activation_temperature = 23.0
  []

  [ASR_expansion]
    type = ConcreteASREigenstrain
    block = 1
    expansion_type = Anisotropic

    reference_temperature = 35.0
    temperature_unit = Celsius
    max_volumetric_expansion = 2.5e-2

    characteristic_time = 35.0
    latency_time = 18.0
    characteristic_activation_energy = 5400.0
    latency_activation_energy = 9400.0
    stress_latency_factor = 1.0

    compressive_strength = 31.0e6
    compressive_stress_exponent = 1.0
    expansion_stress_limit = 8.0e6

    tensile_strength = 3.1e6
    tensile_retention_factor = 1.0
    tensile_absorption_factor = 1.0

    ASR_dependent_tensile_strength = false
    residual_tensile_strength_fraction = 1.0

    temperature = T
    relative_humidity = rh
    rh_exponent = 1.0
    eigenstrain_name = asr_expansion
    absolute_tolerance = 1e-10
    output_iteration_info_on_error = true
  []

  [thermal_strain_concrete]
    type = ComputeThermalExpansionEigenstrain
    block = 1
    temperature = T
    thermal_expansion_coeff = 8.0e-6
    stress_free_temperature = 30.0
    eigenstrain_name = thermal_expansion
  []

  [ASR_damage_concrete]
    type = ConcreteASRMicrocrackingDamage
    residual_youngs_modulus_fraction = 0.7
    damage_index_name = asr_damage_index
    block = 1
  []
  [mazars_damage]
    block = 1
    damage_index_name = mazars_damage_index
    type = MazarsDamage
    tensile_strength = 3.1e6
    a_t = 0.9
    a_c = 1.0
    b_t = 16000
    b_c = 1600
  []
  [combined_damage]
    type = CombinedScalarDamage
    block = 1
    damage_models = 'ASR_damage_concrete mazars_damage'
    damage_index_name = combined_damage_index
    maximum_damage = 0.95
    use_old_damage = true
  []
  [stress]
    type = ComputeMultipleInelasticStress
    block = 1
    inelastic_models = 'creep'
    damage_model = combined_damage
  []

  [plastic_truss]
    type = PlasticTruss
    block = '2'
    youngs_modulus = 2e11
    yield_stress = 415e6
    hardening_constant = 0.0
    relative_tolerance = 1e-5
    absolute_tolerance = 1e-6
    temperature = T
    thermal_expansion_coeff = 11.3e-6
    temperature_ref = 30.0
  []
  [thermal_truss]
    type = GenericConstantMaterial
    block = '2'
    prop_names = 'thermal_conductivity specific_heat density'
    prop_values = '45                 446           7850' # W/(m K), J/(kg K), kg/m^3
  []
[]

[UserObjects]
  [visco_update]
    type = LinearViscoelasticityManager
    block = 1
    viscoelastic_model = logcreep
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = disp_x
    boundary = '2000 2005'
    value = 0.0
  []
  [bottom]
    type = DirichletBC
    variable = disp_y
    boundary = '2000 2001'
    value = 0.0
  []
  [back]
    type = DirichletBC
    variable = disp_z
    boundary = '2000 2005'
    value = 0.0
  []
  [T]
    type = FunctionDirichletBC
    variable = T
    boundary = '101 102 103 104 105 106'
    function = ramp_temp
  []
  [rh]
    type = FunctionDirichletBC
    variable = rh
    boundary = '101 102 103 104 105 106'
    function = ramp_humidity
  []
[]

[Postprocessors]
  [nelem]
    type = NumElems
  []
  [ndof]
    type = NumDOFs
  []
  [ASR_strain]
    type = ElementAverageValue
    variable = ASR_vstrain
    block = 1
  []
  [ASR_strain_xx]
    type = ElementAverageValue
    variable = ASR_strain_xx
    block = 1
  []
  [ASR_strain_yy]
    type = ElementAverageValue
    variable = ASR_strain_yy
    block = 1
  []
  [ASR_strain_zz]
    type = ElementAverageValue
    variable = ASR_strain_zz
    block = 1
  []
  [ASR_ext]
    type = ElementAverageValue
    variable = ASR_ex
    block = 1
  []
  [vonmises]
    type = ElementAverageValue
    variable = vonmises_stress
    block = 1
  []
  [vstrain]
    type = ElementAverageValue
    variable = volumetric_strain
    block = 1
  []
  [strain_xx]
    type = ElementAverageValue
    variable = strain_xx
    block = 1
  []
  [strain_yy]
    type = ElementAverageValue
    variable = strain_yy
    block = 1
  []
  [strain_zz]
    type = ElementAverageValue
    variable = strain_zz
    block = 1
  []
  [temp]
    type = ElementAverageValue
    variable = T
    block = 1
  []
  [humidity]
    type = ElementAverageValue
    variable = rh
    block = 1
  []
  [thermal_strain_xx]
    type = ElementAverageValue
    variable = thermal_strain_xx
    block = 1
  []
  [thermal_strain_yy]
    type = ElementAverageValue
    variable = thermal_strain_yy
    block = 1
  []
  [thermal_strain_zz]
    type = ElementAverageValue
    variable = thermal_strain_zz
    block = 1
  []
  [disp_x_101]
    type = SideAverageValue
    variable = disp_x
    boundary = 101
  []
  [disp_x_102]
    type = SideAverageValue
    variable = disp_x
    boundary = 102
  []
  [disp_x_103]
    type = SideAverageValue
    variable = disp_x
    boundary = 103
  []
  [disp_x_104]
    type = SideAverageValue
    variable = disp_x
    boundary = 104
  []
  [disp_x_105]
    type = SideAverageValue
    variable = disp_x
    boundary = 105
  []
  [disp_x_106]
    type = SideAverageValue
    variable = disp_x
    boundary = 106
  []
  [disp_y_101]
    type = SideAverageValue
    variable = disp_y
    boundary = 101
  []
  [disp_y_102]
    type = SideAverageValue
    variable = disp_y
    boundary = 102
  []
  [disp_y_103]
    type = SideAverageValue
    variable = disp_y
    boundary = 103
  []
  [disp_y_104]
    type = SideAverageValue
    variable = disp_y
    boundary = 104
  []
  [disp_y_105]
    type = SideAverageValue
    variable = disp_y
    boundary = 105
  []
  [disp_y_106]
    type = SideAverageValue
    variable = disp_y
    boundary = 106
  []
  [disp_z_101]
    type = SideAverageValue
    variable = disp_z
    boundary = 101
  []
  [disp_z_102]
    type = SideAverageValue
    variable = disp_z
    boundary = 102
  []
  [disp_z_103]
    type = SideAverageValue
    variable = disp_z
    boundary = 103
  []
  [disp_z_104]
    type = SideAverageValue
    variable = disp_z
    boundary = 104
  []
  [disp_z_105]
    type = SideAverageValue
    variable = disp_z
    boundary = 105
  []
  [disp_z_106]
    type = SideAverageValue
    variable = disp_z
    boundary = 106
  []

  [disp_x_p1_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 -0.08 -0.08'
  []
  [disp_x_p1_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 -0.08 -0.08'
  []
  [disp_x_p2_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 -0.08 0.08'
  []
  [disp_x_p2_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 -0.08 0.08'
  []
  [disp_x_p3_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 0.08 -0.08'
  []
  [disp_x_p3_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 0.08 -0.08'
  []
  [disp_x_p4_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 0.08 0.08'
  []
  [disp_x_p4_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 0.08 0.08'
  []
  [disp_x_p5_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 0.08 -0.235'
  []
  [disp_x_p5_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 0.08 -0.235'
  []
  [disp_x_p6_pos]
    type = PointValue
    variable = disp_x
    point = '0.24 0.08 0.235'
  []
  [disp_x_p6_neg]
    type = PointValue
    variable = disp_x
    point = '-0.24 0.08 0.235'
  []

  [disp_y_p1_pos]
    type = PointValue
    variable = disp_y
    point = '-0.08 0.24 -0.08'
  []
  [disp_y_p1_neg]
    type = PointValue
    variable = disp_y
    point = '-0.08 -0.24 -0.08'
  []
  [disp_y_p2_pos]
    type = PointValue
    variable = disp_y
    point = '-0.08 0.24 0.08'
  []
  [disp_y_p2_neg]
    type = PointValue
    variable = disp_y
    point = '-0.08 -0.24 0.08'
  []
  [disp_y_p3_pos]
    type = PointValue
    variable = disp_y
    point = '0.08 0.24 -0.08'
  []
  [disp_y_p3_neg]
    type = PointValue
    variable = disp_y
    point = '0.08 -0.24 -0.08'
  []
  [disp_y_p4_pos]
    type = PointValue
    variable = disp_y
    point = '0.08 0.24 0.08'
  []
  [disp_y_p4_neg]
    type = PointValue
    variable = disp_y
    point = '0.08 -0.24 0.08'
  []
  [disp_y_p5_pos]
    type = PointValue
    variable = disp_y
    point = '0.08 0.24 -0.235'
  []
  [disp_y_p5_neg]
    type = PointValue
    variable = disp_y
    point = '0.08 -0.24 -0.235'
  []
  [disp_y_p6_pos]
    type = PointValue
    variable = disp_y
    point = '0.08 0.24 0.235'
  []
  [disp_y_p6_neg]
    type = PointValue
    variable = disp_y
    point = '0.08 -0.24 0.235'
  []
  [disp_y_p7_pos]
    type = PointValue
    variable = disp_y
    point = '-0.235 0.24 0.08'
  []
  [disp_y_p7_neg]
    type = PointValue
    variable = disp_y
    point = '-0.235 -0.24 0.08'
  []
  [disp_y_p8_pos]
    type = PointValue
    variable = disp_y
    point = '0.235 0.24 0.08'
  []
  [disp_y_p8_neg]
    type = PointValue
    variable = disp_y
    point = '0.235 -0.24 0.08'
  []

  [disp_z_p1_pos]
    type = PointValue
    variable = disp_z
    point = '-0.08 -0.08 0.24'
  []
  [disp_z_p1_neg]
    type = PointValue
    variable = disp_z
    point = '-0.08 -0.08 -0.24'
  []
  [disp_z_p2_pos]
    type = PointValue
    variable = disp_z
    point = '-0.08 0.08 0.24'
  []
  [disp_z_p2_neg]
    type = PointValue
    variable = disp_z
    point = '-0.08 0.08 -0.24'
  []
  [disp_z_p3_pos]
    type = PointValue
    variable = disp_z
    point = '0.08 -0.08 0.24'
  []
  [disp_z_p3_neg]
    type = PointValue
    variable = disp_z
    point = '0.08 -0.08 -0.24'
  []
  [disp_z_p4_pos]
    type = PointValue
    variable = disp_z
    point = '0.08 0.08 0.24'
  []
  [disp_z_p4_neg]
    type = PointValue
    variable = disp_z
    point = '0.08 0.08 -0.24'
  []
  [disp_z_p5_pos]
    type = PointValue
    variable = disp_z
    point = '0.235 0.08 0.24'
  []
  [disp_z_p5_neg]
    type = PointValue
    variable = disp_z
    point = '0.235 0.08 -0.24'
  []
  [disp_z_p6_pos]
    type = PointValue
    variable = disp_z
    point = '-0.235 0.08 0.24'
  []
  [disp_z_p6_neg]
    type = PointValue
    variable = disp_z
    point = '-0.235 0.08 -0.24'
  []
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  line_search = none
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu superlu_dist'
  start_time = 2419200 #28 days
  dt = 200000
  dtmin = 200000
  automatic_scaling = true
  resid_vs_jac_scaling_param = 0.5
  end_time = 38880000 #450 days
  l_max_its = 10
  nl_max_its = 50
  nl_rel_tol = 1e-5
  nl_abs_tol = 1e-7
  [Predictor]
    type = SimplePredictor
    scale = 1.0
  []
[]

[Outputs]
  perf_graph = true
  csv = true
[]

[Debug]
  show_var_residual_norms = true
[]
