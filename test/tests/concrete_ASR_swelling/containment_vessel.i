[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  volumetric_locking_correction = true
[]

[Mesh]
  file = gold/containment_vessel.e
  construct_side_list_from_node_list = true
  patch_update_strategy=iteration
[]

[Modules/TensorMechanics/Master]
  generate_output = 'stress_xx stress_yy stress_zz stress_xy stress_yz stress_zx
                     strain_xx strain_yy strain_zz strain_xy strain_yz strain_zx
                     vonmises_stress hydrostatic_stress
                     elastic_strain_xx elastic_strain_yy elastic_strain_zz'
  [./concrete]
    strain = FINITE
    block = '1'
    add_variables = true
    eigenstrain_names = 'asr_expansion thermal_expansion'
    save_in = 'resid_x resid_y resid_z'
  [../]
  [./soil]
    strain = FINITE
    block = '12'
    add_variables = true
    save_in = 'resid_x resid_y resid_z'
  [../]
[]

[Modules/TensorMechanics/LineElementMaster]
  [./btm_grid_3]
    block = '8 9 10 11'
    truss = true
    area = area_no3
    displacements = 'disp_x disp_y disp_z'
    save_in = 'resid_x resid_y resid_z'
  [../]
  [./btm_grid_4]
    block = '7'
    truss = true
    area = area_no4
    displacements = 'disp_x disp_y disp_z'
    save_in = 'resid_x resid_y resid_z'
  [../]
  [./btm_grid_5]
    block = '4 5'
    truss = true
    area = area_no5
    displacements = 'disp_x disp_y disp_z'
    save_in = 'resid_x resid_y resid_z'
  [../]
  [./btm_grid_6]
    block = '3 6'
    truss = true
    area = area_no6
    displacements = 'disp_x disp_y disp_z'
    save_in = 'resid_x resid_y resid_z'
  [../]
[]

[Constraints/EqualValueEmbeddedConstraint/EqualValueEmbeddedConstraintAction]
  primary_block = '1'
  secondary_block = '3 4 5 6 7 8 9 10 11'
  primary_variable = 'disp_x disp_y disp_z'
  displacements = 'disp_x disp_y disp_z'
  penalty = 1e12
  formulation = penalty
[]

[Contact]
  [./leftright]
    primary = '6'
    secondary = '5'
    model = frictionless
    formulation = kinematic
    penalty = 1e+12
    normalize_penalty = true
    normal_smoothing_distance = 0.1
  [../]
[]

[Variables]
  [./T]
    order = FIRST
    family = LAGRANGE
    initial_condition = 5.0
    block = '1 3 4 5 6 7 8 9 10 11'
  [../]
  [./rh]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.6
    block = '1'
  [../]
[]

[Kernels]
  [./T_td]
    type     = ConcreteThermalTimeIntegration
    variable = T
    block = '1'
  [../]
  [./T_diff]
    type     = ConcreteThermalConduction
    variable = T
    block = '1'
  [../]
  [./T_conv]
    type     = ConcreteThermalConvection
    variable = T
    relative_humidity = rh
    block = '1'
  [../]
  [./T_adsorption]
    type     = ConcreteLatentHeat
    variable = T
    H = rh
    block = '1'
  [../]
  [./rh_td]
    type     = ConcreteMoistureTimeIntegration
    variable = rh
    block = '1'
  [../]
  [./rh_diff]
    type     = ConcreteMoistureDiffusion
    variable = rh
    temperature = T
    block = '1'
  [../]
  [./heat_dt]
    type = TimeDerivative
    variable = T
    block = '3 4 5 6 7 8 9 10 11'
  [../]
  [./heat_conduction]
    type = HeatConduction
    variable = T
    diffusion_coefficient = 53.0
    block = '3 4 5 6 7 8 9 10 11'
  [../]
  [./gravity]
    type = Gravity
    variable = disp_z
    value = 9.81
  [../]
[]

[AuxVariables]
  [./resid_x]
  [../]
  [./resid_y]
  [../]
  [./resid_z]
  [../]
  [./ASR_ex]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./ASR_vstrain]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./ASR_strain_xx]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./ASR_strain_yy]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./ASR_strain_zz]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./ASR_strain_xy]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./ASR_strain_yz]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./ASR_strain_zx]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./volumetric_strain]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./thermal_strain_xx]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./thermal_strain_yy]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./thermal_strain_zz]
    order = CONSTANT
    family = MONOMIAL
    block = '1'
  [../]
  [./thermal_conductivity]
    order = CONSTANT
    family = Monomial
    block = '1'
  [../]
  [./thermal_capacity]
    order = CONSTANT
    family = Monomial
    block = '1'
  [../]
  [./moisture_capacity]
    order = CONSTANT
    family = Monomial
    block = '1'
  [../]
  [./humidity_diffusivity]
    order = CONSTANT
    family = Monomial
    block = '1'
  [../]
  [./water_content]
    order = CONSTANT
    family = Monomial
    block = '1'
  [../]
  [./water_hydrated]
    order = CONSTANT
    family = Monomial
    block = '1'
  [../]
  [damage_index]
    order = CONSTANT
    family = Monomial
    block = '1'
  []

  [./area_no3]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./area_no4]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./area_no5]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./area_no6]
    order = CONSTANT
    family = MONOMIAL
  [../]

  # [./stress_xx_soil]
  #   order = CONSTANT
  #   family = MONOMIAL
  #   block = '12'
  # [../]
  # [./stress_xy_soil]
  #   order = CONSTANT
  #   family = MONOMIAL
  #   block = '12'
  # [../]
  # [./stress_xz_soil]
  #   order = CONSTANT
  #   family = MONOMIAL
  #   block = '12'
  # [../]
  # [./stress_yy_soil]
  #   order = CONSTANT
  #   family = MONOMIAL
  #   block = '12'
  # [../]
  # [./stress_yz_soil]
  #   order = CONSTANT
  #   family = MONOMIAL
  #   block = '12'
  # [../]
  # [./stress_zz_soil]
  #   order = CONSTANT
  #   family = MONOMIAL
  #   block = '12'
  # [../]
  # [./mc_int]
  #   order = CONSTANT
  #   family = MONOMIAL
  #   block = '12'
  # [../]
  # [./yield_fcn]
  #   order = CONSTANT
  #   family = MONOMIAL
  #   block = '12'
  # [../]
[]

[AuxKernels]
  [./ASR_ex]
    type = MaterialRealAux
    variable = ASR_ex
    block = '1'
    property = ASR_extent
    execute_on = 'timestep_end'
  [../]
  [./ASR_vstrain]
    type = MaterialRealAux
    block = '1'
    variable = ASR_vstrain
    property = ASR_volumetric_strain
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_xx]
    type = RankTwoAux
    block = '1'
    rank_two_tensor = asr_expansion
    variable = ASR_strain_xx
    index_i = 0
    index_j = 0
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_yy]
    type = RankTwoAux
    block = '1'
    rank_two_tensor = asr_expansion
    variable = ASR_strain_yy
    index_i = 1
    index_j = 1
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_zz]
    type = RankTwoAux
    block = '1'
    rank_two_tensor = asr_expansion
    variable = ASR_strain_zz
    index_i = 2
    index_j = 2
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_xy]
    type = RankTwoAux
    block = '1'
    rank_two_tensor = asr_expansion
    variable = ASR_strain_xy
    index_i = 0
    index_j = 1
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_yz]
    type = RankTwoAux
    block = '1'
    rank_two_tensor = asr_expansion
    variable = ASR_strain_yz
    index_i = 1
    index_j = 2
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_zx]
    type = RankTwoAux
    block = '1'
    rank_two_tensor = asr_expansion
    variable = ASR_strain_zx
    index_i = 0
    index_j = 2
    execute_on = 'timestep_end'
  [../]
  [./thermal_strain_xx]
    type = RankTwoAux
    block = '1'
    rank_two_tensor = thermal_expansion
    variable = thermal_strain_xx
    index_i = 0
    index_j = 0
    execute_on = 'timestep_end'
  [../]
  [./thermal_strain_yy]
    type = RankTwoAux
    block = '1'
    rank_two_tensor = thermal_expansion
    variable = thermal_strain_yy
    index_i = 1
    index_j = 1
    execute_on = 'timestep_end'
  [../]
  [./thermal_strain_zz]
    type = RankTwoAux
    block = '1'
    rank_two_tensor = thermal_expansion
    variable = thermal_strain_zz
    index_i = 2
    index_j = 2
    execute_on = 'timestep_end'
  [../]
  [./volumetric_strain]
    type = RankTwoScalarAux
    scalar_type = VolumetricStrain
    rank_two_tensor = total_strain
    variable = volumetric_strain
    block = '1'
  [../]
  [./k]
    type = MaterialRealAux
    variable = thermal_conductivity
    property = thermal_conductivity
    execute_on = 'timestep_end'
    block = '1'
  [../]
  [./capacity]
    type = MaterialRealAux
    variable = thermal_capacity
    property = thermal_capacity
    execute_on = 'timestep_end'
    block = '1'
  [../]
  [./rh_capacity]
    type = MaterialRealAux
    variable = moisture_capacity
    property = moisture_capacity
    execute_on = 'timestep_end'
    block = '1'
  [../]
  [./rh_duff]
    type = MaterialRealAux
    variable = humidity_diffusivity
    property = humidity_diffusivity
    execute_on = 'timestep_end'
    block = '1'
  [../]
  [damage_index]
    type = MaterialRealAux
    block = '1'
    variable = damage_index
    property = damage_index
    execute_on = timestep_end
  []
  [./area_no3]
    type = ConstantAux
    # block = '2'
    block = '7'
    variable = area_no3
    value = 426e-6
    execute_on = 'initial timestep_begin'
  [../]
  [./area_no4]
    type = ConstantAux
    # block = '2'
    block = '8 9 10 11'
    variable = area_no4
    value = 774e-6
    execute_on = 'initial timestep_begin'
  [../]
  [./area_no5]
    type = ConstantAux
    # block = '2'
    block = '4 5'
    variable = area_no5
    value = 1200e-6
    execute_on = 'initial timestep_begin'
  [../]
  [./area_no6]
    type = ConstantAux
    # block = '2'
    block = '3 6'
    variable = area_no6
    value = 1704e-6
    execute_on = 'initial timestep_begin'
  [../]
  # [./stress_xx]
  #   type = RankTwoAux
  #   rank_two_tensor = stress
  #   variable = stress_xx_soil
  #   block = '12'
  #   index_i = 0
  #   index_j = 0
  # [../]
  # [./stress_xy]
  #   type = RankTwoAux
  #   rank_two_tensor = stress
  #   variable = stress_xy_soil
  #   block = '12'
  #   index_i = 0
  #   index_j = 1
  # [../]
  # [./stress_xz]
  #   type = RankTwoAux
  #   rank_two_tensor = stress
  #   variable = stress_xz_soil
  #   block = '12'
  #   index_i = 0
  #   index_j = 2
  # [../]
  # [./stress_yy]
  #   type = RankTwoAux
  #   rank_two_tensor = stress
  #   variable = stress_yy_soil
  #   block = '12'
  #   index_i = 1
  #   index_j = 1
  # [../]
  # [./stress_yz]
  #   type = RankTwoAux
  #   rank_two_tensor = stress
  #   variable = stress_yz_soil
  #   block = '12'
  #   index_i = 1
  #   index_j = 2
  # [../]
  # [./stress_zz]
  #   type = RankTwoAux
  #   rank_two_tensor = stress
  #   variable = stress_zz_soil
  #   block = '12'
  #   index_i = 2
  #   index_j = 2
  # [../]
  # [./mc_int_auxk]
  #   type = MaterialStdVectorAux
  #   index = 0
  #   property = plastic_internal_parameter
  #   variable = mc_int
  #   block = '12'
  # [../]
  # [./yield_fcn_auxk]
  #   type = MaterialStdVectorAux
  #   index = 0
  #   property = plastic_yield_function
  #   variable = yield_fcn
  #   block = '12'
  # [../]
[]

[Functions]
  [./T_air]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_climate_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 1
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
  [./rh_air]
     type = PiecewiseLinear
     data_file =  gold/containment_vessel_climate_data_crossville.csv
     x_index_in_file = 0
     y_index_in_file = 2
     format = columns
     xy_in_file_only = false
     direction = right
  [../]
  [./rh_bet_grnd_2in]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_soil_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 1
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
  [./T_bet_grnd_2in]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_soil_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 2
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
  [./rh_bet_2in_4in]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_soil_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 3
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
  [./T_bet_2in_4in]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_soil_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 4
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
  [./rh_bet_4in_8in]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_soil_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 5
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
  [./T_bet_4in_8in]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_soil_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 6
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
  [./rh_bet_8in_20in]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_soil_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 7
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
  [./T_bet_8in_20in]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_soil_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 8
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
  [./rh_below_20in]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_soil_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 9
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
  [./T_below_20in]
    type = PiecewiseLinear
    data_file =  gold/containment_vessel_soil_data_crossville.csv
    x_index_in_file = 0
    y_index_in_file = 10
    format = columns
    xy_in_file_only = false
    direction = right
  [../]
[]

[BCs]
  [./x_disp]
    type = DirichletBC
    variable = disp_x
    boundary = '1'
    value    = 0.0
  [../]
  [./y_disp]
    type = DirichletBC
    variable = disp_y
    boundary = '2'
    value    = 0.0
  [../]
  [./z_disp]
    type = DirichletBC
    variable = disp_z
    boundary = '3'
    value    = 0.0
  [../]
  # [./T_disp]
  #   type = DirichletBC
  #   variable = T
  #   boundary = '11'
  #   value    = 18.0
  # [../]
  # [./RH_disp]
  #   type = DirichletBC
  #   variable = rh
  #   boundary = '11'
  #   value    = 0.6
  # [../]
  [./T_air]
    type = RepeatingAngularTemperatureBC
    variable = T
    boundary = '11'
    repetition_period = 31536000 # 365 days
    dT_sun_shade = 16.5
    T_air = T_air
  [../]
  [./rh_air]
    type = RepeatingDirichletBC
    variable = rh
    boundary = '11'
    repetition_period = 31536000 # 365 days
    function = rh_air
  [../]
  # [./T_bet_grnd_2in]
  #   type = RepeatingDirichletBC
  #   variable = T
  #   boundary = '12'
  #   repetition_period = 31536000 # 365 days
  #   function = T_bet_grnd_2in
  # [../]
  # [./rh_bet_grnd_2in]
  #   type = RepeatingDirichletBC
  #   variable = rh
  #   boundary = '12'
  #   repetition_period = 31536000 # 365 days
  #   function = rh_bet_grnd_2in
  # [../]
  # [./T_bet_2in_4in]
  #   type = RepeatingDirichletBC
  #   variable = T
  #   boundary = '13'
  #   repetition_period = 31536000 # 365 days
  #   function = T_bet_2in_4in
  # [../]
  # [./rh_bet_2in_4in]
  #   type = RepeatingDirichletBC
  #   variable = rh
  #   boundary = '13'
  #   repetition_period = 31536000 # 365 days
  #   function = rh_bet_2in_4in
  # [../]
  # [./T_bet_4in_8in]
  #   type = RepeatingDirichletBC
  #   variable = T
  #   boundary = '14'
  #   repetition_period = 31536000 # 365 days
  #   function = T_bet_4in_8in
  # [../]
  # [./rh_bet_4in_8in]
  #   type = RepeatingDirichletBC
  #   variable = rh
  #   boundary = '14'
  #   repetition_period = 31536000 # 365 days
  #   function = rh_bet_4in_8in
  # [../]
  # [./T_bet_8in_20in]
  #   type = RepeatingDirichletBC
  #   variable = T
  #   boundary = '15'
  #   repetition_period = 31536000 # 365 days
  #   function = T_bet_8in_20in
  # [../]
  # [./rh_bet_8in_20in]
  #   type = RepeatingDirichletBC
  #   variable = rh
  #   boundary = '15'
  #   repetition_period = 31536000 # 365 days
  #   function = rh_bet_8in_20in
  # [../]
  # [./T_below_20in]
  #   type = RepeatingDirichletBC
  #   variable = T
  #   boundary = '16'
  #   repetition_period = 31536000 # 365 days
  #   function = T_below_20in
  # [../]
  # [./rh_below_20in]
  #   type = RepeatingDirichletBC
  #   variable = rh
  #   boundary = '16'
  #   repetition_period = 31536000 # 365 days
  #   function = rh_below_20in
  # [../]
  [./T_below_20in_17]
    type = FunctionDirichletBC
    # type = RepeatingDirichletBC
    variable = T
    boundary = '17'
    # repetition_period = 31536000 # 365 days
    function = T_below_20in
  [../]
  [./rh_below_water_table]
    type = FunctionDirichletBC
    variable = rh
    boundary = '17'
    function = '1.0'
  [../]
[]

[Materials]
  [./concrete]
    type = ConcreteThermalMoisture
    block = '1'
    # setup thermal transport models and parameters
    # options available: CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004 KIM-2003
    thermal_transport_model = KODUR-2004
    aggregate_type = Siliceous               #options: Siliceous Carbonate

    # setup moisture transport models and parameters
    # options available: Bazant Mensi Xi
    moisture_transport_model = Xi
    cement_type = 2 # options: 1 2 3 4
    aggregate_vol_fraction = 0.7 # used in Xi's moisture diffusivity model
    concrete_cure_time = 14.0 # curing time in (days)
    aggregate_pore_type = dense # options: dense porous

    # concrete mix parameters
    cement_mass = 354.0 #mass of cement (kg) per m^3 of concrete
    aggregate_mass = 1877.0 #mass of aggregate (kg) per m^3 of concrete
    water_to_cement_ratio = 0.53
    ref_density_of_concrete = 2231.0 # in kg/m^3
    ref_specific_heat_of_concrete = 1100.0 # in J/(Kg.0C)
    ref_thermal_conductivity_of_concrete = 3 # in W/(m.0C)

    # coupled nonlinear variables
    relative_humidity                    = rh
    temperature                          = T
  [../]
  [./creep]
    type = LinearViscoelasticStressUpdate
    block = '1'
  [../]
  [burgers]
    type = GeneralizedKelvinVoigtModel
    block = '1'
    creep_modulus = '1.52e12
                     5.32e18
                     6.15e10
                     6.86e10
                     4.48e10
                     1.05e128' # data from TAMU
    creep_viscosity = '1
                       10
                       100
                       1000
                       10000
                       100000'  # data from TAMU
    poisson_ratio = 0.2
    young_modulus = 27.8e9 #33.03e9 Lower value from ACI eqn
  []
  [ASR_expansion]
    type = ConcreteASREigenstrain
    block = '1'
    expansion_type = Anisotropic

    reference_temperature = 23.0 # parameter to play
    temperature_unit = Celsius
    max_volumetric_expansion = 1.125e-2 # parameter to play

    characteristic_time = 100 # parameter to play
    latency_time = 50 # parameter to play
    characteristic_activation_energy = 5400.0
    latency_activation_energy = 9400.0
    stress_latency_factor = 1.0

    compressive_strength = 46.9e6
    compressive_stress_exponent = 0.0
    expansion_stress_limit = 8.0e6

    tensile_strength = 3.45e6
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
    max_its = 100
  []
  [thermal_strain_concrete]
    type = ComputeThermalExpansionEigenstrain
    block = '1'
    temperature = T
    thermal_expansion_coeff = 8.0e-6
    stress_free_temperature = 23.0
    eigenstrain_name = thermal_expansion
  []
  [ASR_damage_concrete]
    type = ConcreteASRMicrocrackingDamage
    block = '1'
    residual_youngs_modulus_fraction = 0.1
  []
  [./stress_concrete]
    type = ComputeMultipleInelasticStress
    block = '1'
    inelastic_models = 'creep'
    damage_model = ASR_damage_concrete
  [../]
  [./density_conc]
    type = GenericFunctionMaterial
    block = '1'
    prop_names = density
    prop_values = 2231.0 # kg/m3
  [../]
  [truss]
    type = LinearElasticTruss
    block = '3 4 5 6 7 8 9 10 11'
    youngs_modulus = 2.14e11
    temperature = T
    thermal_expansion_coeff = 11.3e-6
    temperature_ref = 23.0
  []
  [./density_steel]
    type = GenericFunctionMaterial
    block = '3 4 5 6 7 8 9 10 11'
    prop_names = density
    prop_values = 7850.0 # kg/m3
  [../]
  [elastic_soil]
    type = ComputeElasticityTensor
    fill_method = symmetric_isotropic
    C_ijkl = '0 1E7'
    block = '12'
  []
  [./mc_soil_stress]
    type = ComputeMultiPlasticityStress
    block = '12'
    ep_plastic_tolerance = 1E-11
    plastic_models = mc
    max_NR_iterations = 1000
    debug_fspb = crash
  [../]
  [./density_soil]
    type = GenericFunctionMaterial
    block = '12'
    prop_names = density
    prop_values = 2650.0 # kg/m3
  [../]
[]

[UserObjects]
  [./visco_update]
    type = LinearViscoelasticityManager
    block = '1'
    viscoelastic_model = burgers
  [../]
  [./mc_coh]
    type = TensorMechanicsHardeningConstant
    value = 10E6
  [../]
  [./mc_phi]
    type = TensorMechanicsHardeningConstant
    value = 40
    convert_to_radians = true
  [../]
  [./mc_psi]
    type = TensorMechanicsHardeningConstant
    value = 40
    convert_to_radians = true
  [../]
  [./mc]
    type = TensorMechanicsPlasticMohrCoulomb
    cohesion = mc_coh
    friction_angle = mc_phi
    dilation_angle = mc_psi
    mc_tip_smoother = 0.01E6
    mc_edge_smoother = 29
    yield_function_tolerance = 1E-5
    internal_constraint_tolerance = 1E-11
  [../]
[]

[Postprocessors]
  [./base_shade_ASR_ex]
    type = PointValue
    variable = ASR_ex
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_ASR_strainxx]
    type = PointValue
    variable = ASR_strain_xx
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_ASR_strainxy]
    type = PointValue
    variable = ASR_strain_xy
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_ASR_strainxz]
    type = PointValue
    variable = ASR_strain_zx
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_ASR_strainyy]
    type = PointValue
    variable = ASR_strain_yy
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_ASR_strainyz]
    type = PointValue
    variable = ASR_strain_yz
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_ASR_strainzz]
    type = PointValue
    variable = ASR_strain_zz
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_ASR_vstrain]
    type = PointValue
    variable = ASR_vstrain
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_T]
    type = PointValue
    variable = T
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_rh]
    type = PointValue
    variable = rh
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_damage_index]
    type = PointValue
    variable = damage_index
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_hydrostatic_stress]
    type = PointValue
    variable = hydrostatic_stress
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_vonmises_stress]
    type = PointValue
    variable = vonmises_stress
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_water_content]
    type = PointValue
    variable = water_content
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_water_hydrated]
    type = PointValue
    variable = water_hydrated
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_thermal_capacity]
    type = PointValue
    variable = thermal_capacity
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_thermal_conductivity]
    type = PointValue
    variable = thermal_conductivity
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_thermal_strain_xx]
    type = PointValue
    variable = thermal_strain_xx
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_thermal_strain_yy]
    type = PointValue
    variable = thermal_strain_yy
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_thermal_strain_zz]
    type = PointValue
    variable = thermal_strain_zz
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_humidity_diffusivity]
    type = PointValue
    variable = humidity_diffusivity
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]
  [./base_shade_moisture_capacity]
    type = PointValue
    variable = moisture_capacity
    point = '15.90 15.90 0.15'# (22.36 45)
  [../]

  [./cyl_air_shade_ASR_ex]
    type = PointValue
    variable = ASR_ex
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_ASR_strainxx]
    type = PointValue
    variable = ASR_strain_xx
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_ASR_strainxy]
    type = PointValue
    variable = ASR_strain_xy
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_ASR_strainxz]
    type = PointValue
    variable = ASR_strain_zx
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_ASR_strainyy]
    type = PointValue
    variable = ASR_strain_yy
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_ASR_strainyz]
    type = PointValue
    variable = ASR_strain_yz
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_ASR_strainzz]
    type = PointValue
    variable = ASR_strain_zz
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_ASR_vstrain]
    type = PointValue
    variable = ASR_vstrain
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_T]
    type = PointValue
    variable = T
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_rh]
    type = PointValue
    variable = rh
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_damage_index]
    type = PointValue
    variable = damage_index
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_hydrostatic_stress]
    type = PointValue
    variable = hydrostatic_stress
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_vonmises_stress]
    type = PointValue
    variable = vonmises_stress
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_water_content]
    type = PointValue
    variable = water_content
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_water_hydrated]
    type = PointValue
    variable = water_hydrated
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_thermal_capacity]
    type = PointValue
    variable = thermal_capacity
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_thermal_conductivity]
    type = PointValue
    variable = thermal_conductivity
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_thermal_strain_xx]
    type = PointValue
    variable = thermal_strain_xx
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_thermal_strain_yy]
    type = PointValue
    variable = thermal_strain_yy
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_thermal_strain_zz]
    type = PointValue
    variable = thermal_strain_zz
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_humidity_diffusivity]
    type = PointValue
    variable = humidity_diffusivity
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_shade_moisture_capacity]
    type = PointValue
    variable = moisture_capacity
    point = '14.80 14.80 32.792'# (22.36 45)
  [../]

  [./cyl_gnd_shade_ASR_ex]
    type = PointValue
    variable = ASR_ex
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_ASR_strainxx]
    type = PointValue
    variable = ASR_strain_xx
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_ASR_strainxy]
    type = PointValue
    variable = ASR_strain_xy
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_ASR_strainxz]
    type = PointValue
    variable = ASR_strain_zx
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_ASR_strainyy]
    type = PointValue
    variable = ASR_strain_yy
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_ASR_strainyz]
    type = PointValue
    variable = ASR_strain_yz
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_ASR_strainzz]
    type = PointValue
    variable = ASR_strain_zz
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_ASR_vstrain]
    type = PointValue
    variable = ASR_vstrain
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_T]
    type = PointValue
    variable = T
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_rh]
    type = PointValue
    variable = rh
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_damage_index]
    type = PointValue
    variable = damage_index
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_hydrostatic_stress]
    type = PointValue
    variable = hydrostatic_stress
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_vonmises_stress]
    type = PointValue
    variable = vonmises_stress
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_water_content]
    type = PointValue
    variable = water_content
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_water_hydrated]
    type = PointValue
    variable = water_hydrated
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_thermal_capacity]
    type = PointValue
    variable = thermal_capacity
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_thermal_conductivity]
    type = PointValue
    variable = thermal_conductivity
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_thermal_strain_xx]
    type = PointValue
    variable = thermal_strain_xx
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_thermal_strain_yy]
    type = PointValue
    variable = thermal_strain_yy
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_thermal_strain_zz]
    type = PointValue
    variable = thermal_strain_zz
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_humidity_diffusivity]
    type = PointValue
    variable = humidity_diffusivity
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_shade_moisture_capacity]
    type = PointValue
    variable = moisture_capacity
    point = '14.80 14.80 10.596'# (22.36 45)
  [../]

  [./dome_shade_ASR_ex]
    type = PointValue
    variable = ASR_ex
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_ASR_strainxx]
    type = PointValue
    variable = ASR_strain_xx
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_ASR_strainxy]
    type = PointValue
    variable = ASR_strain_xy
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_ASR_strainxz]
    type = PointValue
    variable = ASR_strain_zx
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_ASR_strainyy]
    type = PointValue
    variable = ASR_strain_yy
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_ASR_strainyz]
    type = PointValue
    variable = ASR_strain_yz
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_ASR_strainzz]
    type = PointValue
    variable = ASR_strain_zz
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_ASR_vstrain]
    type = PointValue
    variable = ASR_vstrain
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_T]
    type = PointValue
    variable = T
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_rh]
    type = PointValue
    variable = rh
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_damage_index]
    type = PointValue
    variable = damage_index
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_hydrostatic_stress]
    type = PointValue
    variable = hydrostatic_stress
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_vonmises_stress]
    type = PointValue
    variable = vonmises_stress
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_water_content]
    type = PointValue
    variable = water_content
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_water_hydrated]
    type = PointValue
    variable = water_hydrated
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_thermal_capacity]
    type = PointValue
    variable = thermal_capacity
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_thermal_conductivity]
    type = PointValue
    variable = thermal_conductivity
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_thermal_strain_xx]
    type = PointValue
    variable = thermal_strain_xx
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_thermal_strain_yy]
    type = PointValue
    variable = thermal_strain_yy
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_thermal_strain_zz]
    type = PointValue
    variable = thermal_strain_zz
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_humidity_diffusivity]
    type = PointValue
    variable = humidity_diffusivity
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]
  [./dome_shade_moisture_capacity]
    type = PointValue
    variable = moisture_capacity
    point = '10.37 10.37 58.857'# (22.36 45)
  [../]

  [./base_sun_ASR_ex]
    type = PointValue
    variable = ASR_ex
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_ASR_strainxx]
    type = PointValue
    variable = ASR_strain_xx
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_ASR_strainxy]
    type = PointValue
    variable = ASR_strain_xy
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_ASR_strainxz]
    type = PointValue
    variable = ASR_strain_zx
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_ASR_strainyy]
    type = PointValue
    variable = ASR_strain_yy
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_ASR_strainyz]
    type = PointValue
    variable = ASR_strain_yz
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_ASR_strainzz]
    type = PointValue
    variable = ASR_strain_zz
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_ASR_vstrain]
    type = PointValue
    variable = ASR_vstrain
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_T]
    type = PointValue
    variable = T
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_rh]
    type = PointValue
    variable = rh
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_damage_index]
    type = PointValue
    variable = damage_index
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_hydrostatic_stress]
    type = PointValue
    variable = hydrostatic_stress
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_vonmises_stress]
    type = PointValue
    variable = vonmises_stress
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_water_content]
    type = PointValue
    variable = water_content
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_water_hydrated]
    type = PointValue
    variable = water_hydrated
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_thermal_capacity]
    type = PointValue
    variable = thermal_capacity
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_thermal_conductivity]
    type = PointValue
    variable = thermal_conductivity
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_thermal_strain_xx]
    type = PointValue
    variable = thermal_strain_xx
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_thermal_strain_yy]
    type = PointValue
    variable = thermal_strain_yy
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_thermal_strain_zz]
    type = PointValue
    variable = thermal_strain_zz
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_humidity_diffusivity]
    type = PointValue
    variable = humidity_diffusivity
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]
  [./base_sun_moisture_capacity]
    type = PointValue
    variable = moisture_capacity
    point = '15.90 -15.90 0.15'# (22.36 45)
  [../]

  [./cyl_air_sun_ASR_ex]
    type = PointValue
    variable = ASR_ex
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_ASR_strainxx]
    type = PointValue
    variable = ASR_strain_xx
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_ASR_strainxy]
    type = PointValue
    variable = ASR_strain_xy
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_ASR_strainxz]
    type = PointValue
    variable = ASR_strain_zx
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_ASR_strainyy]
    type = PointValue
    variable = ASR_strain_yy
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_ASR_strainyz]
    type = PointValue
    variable = ASR_strain_yz
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_ASR_strainzz]
    type = PointValue
    variable = ASR_strain_zz
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_ASR_vstrain]
    type = PointValue
    variable = ASR_vstrain
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_T]
    type = PointValue
    variable = T
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_rh]
    type = PointValue
    variable = rh
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_damage_index]
    type = PointValue
    variable = damage_index
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_hydrostatic_stress]
    type = PointValue
    variable = hydrostatic_stress
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_vonmises_stress]
    type = PointValue
    variable = vonmises_stress
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_water_content]
    type = PointValue
    variable = water_content
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_water_hydrated]
    type = PointValue
    variable = water_hydrated
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_thermal_capacity]
    type = PointValue
    variable = thermal_capacity
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_thermal_conductivity]
    type = PointValue
    variable = thermal_conductivity
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_thermal_strain_xx]
    type = PointValue
    variable = thermal_strain_xx
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_thermal_strain_yy]
    type = PointValue
    variable = thermal_strain_yy
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_thermal_strain_zz]
    type = PointValue
    variable = thermal_strain_zz
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_humidity_diffusivity]
    type = PointValue
    variable = humidity_diffusivity
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]
  [./cyl_air_sun_moisture_capacity]
    type = PointValue
    variable = moisture_capacity
    point = '14.80 -14.80 32.792'# (22.36 45)
  [../]

  [./cyl_gnd_sun_ASR_ex]
    type = PointValue
    variable = ASR_ex
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_ASR_strainxx]
    type = PointValue
    variable = ASR_strain_xx
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_ASR_strainxy]
    type = PointValue
    variable = ASR_strain_xy
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_ASR_strainxz]
    type = PointValue
    variable = ASR_strain_zx
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_ASR_strainyy]
    type = PointValue
    variable = ASR_strain_yy
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_ASR_strainyz]
    type = PointValue
    variable = ASR_strain_yz
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_ASR_strainzz]
    type = PointValue
    variable = ASR_strain_zz
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_ASR_vstrain]
    type = PointValue
    variable = ASR_vstrain
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_T]
    type = PointValue
    variable = T
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_rh]
    type = PointValue
    variable = rh
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_damage_index]
    type = PointValue
    variable = damage_index
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_hydrostatic_stress]
    type = PointValue
    variable = hydrostatic_stress
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_vonmises_stress]
    type = PointValue
    variable = vonmises_stress
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_water_content]
    type = PointValue
    variable = water_content
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_water_hydrated]
    type = PointValue
    variable = water_hydrated
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_thermal_capacity]
    type = PointValue
    variable = thermal_capacity
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_thermal_conductivity]
    type = PointValue
    variable = thermal_conductivity
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_thermal_strain_xx]
    type = PointValue
    variable = thermal_strain_xx
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_thermal_strain_yy]
    type = PointValue
    variable = thermal_strain_yy
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_thermal_strain_zz]
    type = PointValue
    variable = thermal_strain_zz
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_humidity_diffusivity]
    type = PointValue
    variable = humidity_diffusivity
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]
  [./cyl_gnd_sun_moisture_capacity]
    type = PointValue
    variable = moisture_capacity
    point = '14.80 -14.80 10.596'# (22.36 45)
  [../]

  [./dome_sun_ASR_ex]
    type = PointValue
    variable = ASR_ex
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_ASR_strainxx]
    type = PointValue
    variable = ASR_strain_xx
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_ASR_strainxy]
    type = PointValue
    variable = ASR_strain_xy
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_ASR_strainxz]
    type = PointValue
    variable = ASR_strain_zx
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_ASR_strainyy]
    type = PointValue
    variable = ASR_strain_yy
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_ASR_strainyz]
    type = PointValue
    variable = ASR_strain_yz
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_ASR_strainzz]
    type = PointValue
    variable = ASR_strain_zz
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_ASR_vstrain]
    type = PointValue
    variable = ASR_vstrain
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_T]
    type = PointValue
    variable = T
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_rh]
    type = PointValue
    variable = rh
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_damage_index]
    type = PointValue
    variable = damage_index
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_hydrostatic_stress]
    type = PointValue
    variable = hydrostatic_stress
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_vonmises_stress]
    type = PointValue
    variable = vonmises_stress
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_water_content]
    type = PointValue
    variable = water_content
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_water_hydrated]
    type = PointValue
    variable = water_hydrated
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_thermal_capacity]
    type = PointValue
    variable = thermal_capacity
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_thermal_conductivity]
    type = PointValue
    variable = thermal_conductivity
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_thermal_strain_xx]
    type = PointValue
    variable = thermal_strain_xx
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_thermal_strain_yy]
    type = PointValue
    variable = thermal_strain_yy
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_thermal_strain_zz]
    type = PointValue
    variable = thermal_strain_zz
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_humidity_diffusivity]
    type = PointValue
    variable = humidity_diffusivity
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]
  [./dome_sun_moisture_capacity]
    type = PointValue
    variable = moisture_capacity
    point = '10.37 -10.37 58.857'# (22.36 45)
  [../]

  [./base_shade_rx0]
    type = PointValue
    variable = disp_x
    point = '15.804 15.804 0.15'# (22.36 45)
  [../]
  [./base_shade_rx1]
    type = PointValue
    variable = disp_x
    point = '16 16 0.15'# (22.86 45)
  [../]
  [./base_shade_ry0]
    type = PointValue
    variable = disp_y
    point = '15.804 15.804 0.15'# (22.36 45)
  [../]
  [./base_shade_ry1]
    type = PointValue
    variable = disp_y
    point = '16 16 0.15'# (22.86 45)
  [../]
  [./base_shade_thx0]
    type = PointValue
    variable = disp_x
    point = '14.366 17.122 0.15' # (22.86 40)
  [../]
  [./base_shade_thx1]
    type = PointValue
    variable = disp_x
    point = '17.122 14.366 0.15' # (22.86 50)
  [../]
  [./base_shade_thy0]
    type = PointValue
    variable = disp_y
    point = '14.366 17.122 0.15' # (22.86 40)
  [../]
  [./base_shade_thy1]
    type = PointValue
    variable = disp_y
    point = '17.122 14.366 0.15' # (22.86 50)
  [../]
  [./base_shade_z]
    type = AveragePointSeparation
    displacements = 'disp_z'
    first_point = '14.694 17.511 -2.00'
    last_point =  '14.694 17.511  3.00'
  [../]

  [./base_sun_rx0]
    type = PointValue
    variable = disp_x
    point = '15.804 -15.804 0.15'# (22.36 -45)
  [../]
  [./base_sun_rx1]
    type = PointValue
    variable = disp_x
    point = '16 -16 0.15'# (22.86 -45)
  [../]
  [./base_sun_ry0]
    type = PointValue
    variable = disp_y
    point = '15.804 -15.804 0.15'# (22.36 -45)
  [../]
  [./base_sun_ry1]
    type = PointValue
    variable = disp_y
    point = '16 -16 0.15'# (22.86 -45)
  [../]
  [./base_sun_thx0]
    type = PointValue
    variable = disp_x
    point = '14.366 -17.122 0.15' # (22.86 -40)
  [../]
  [./base_sun_thx1]
    type = PointValue
    variable = disp_x
    point = '17.122 -14.366 0.15' # (22.86 -50)
  [../]
  [./base_sun_thy0]
    type = PointValue
    variable = disp_y
    point = '14.366 -17.122 0.15' # (22.86 -40)
  [../]
  [./base_sun_thy1]
    type = PointValue
    variable = disp_y
    point = '17.122 -14.366 0.15' # (22.86 -50)
  [../]
  [./base_sun_z]
    type = AveragePointSeparation
    displacements = 'disp_z'
    first_point = '14.694 -17.511 -2.00'
    last_point =  '14.694 -17.511  3.00'
  [../]

  [./cyl_air_shade_rx0]
    type = PointValue
    variable = disp_x
    point = '14.713 14.713 32.792'# (20.80 45)
  [../]
  [./cyl_air_shade_rx1]
    type = PointValue
    variable = disp_x
    point = '14.9 14.9 32.792'# (21.30 45)
  [../]
  [./cyl_air_shade_ry0]
    type = PointValue
    variable = disp_y
    point = '14.713 14.713 32.792'# (20.80 45)
  [../]
  [./cyl_air_shade_ry1]
    type = PointValue
    variable = disp_y
    point = '14.9 14.9 32.792'# (21.30 45)
  [../]
  [./cyl_air_shade_thx0]
    type = PointValue
    variable = disp_x
    point = '16.321 13.600 32.792'# (21.30 40)
  [../]
  [./cyl_air_shade_thx1]
    type = PointValue
    variable = disp_x
    point = '13.600 16.321 32.792'# (21.30 50)
  [../]
  [./cyl_air_shade_thy0]
    type = PointValue
    variable = disp_y
    point = '16.321 13.600 32.792'# (21.30 40)
  [../]
  [./cyl_air_shade_thy1]
    type = PointValue
    variable = disp_y
    point = '13.600 16.321 32.792'# (21.30 50)
  [../]
  [./cyl_air_shade_z]
    type = AveragePointSeparation
    displacements = 'disp_z'
    first_point = '14.9 14.9 32.542'# (21.30 45)
    last_point =  '14.9 14.9 33.042'# (21.30 45)
  [../]

  [./cyl_air_sun_rx0]
    type = PointValue
    variable = disp_x
    point = '14.713 -14.713 32.792'# (20.80 -45)
  [../]
  [./cyl_air_sun_rx1]
    type = PointValue
    variable = disp_x
    point = '14.9 -14.9 32.792'# (21.30 -45)
  [../]
  [./cyl_air_sun_ry0]
    type = PointValue
    variable = disp_y
    point = '14.713 -14.713 32.792'# (20.80 -45)
  [../]
  [./cyl_air_sun_ry1]
    type = PointValue
    variable = disp_y
    point = '14.9 -14.9 32.792'# (21.30 -45)
  [../]
  [./cyl_air_sun_thx0]
    type = PointValue
    variable = disp_x
    point = '16.321 -13.600 32.792'# (21.30 -40)
  [../]
  [./cyl_air_sun_thx1]
    type = PointValue
    variable = disp_x
    point = '13.600 -16.321 32.792'# (21.30 -50)
  [../]
  [./cyl_air_sun_thy0]
    type = PointValue
    variable = disp_y
    point = '16.321 -13.600 32.792'# (21.30 -40)
  [../]
  [./cyl_air_sun_thy1]
    type = PointValue
    variable = disp_y
    point = '13.600 -16.321 32.792'# (21.30 -50)
  [../]
  [./cyl_air_sun_z]
    type = AveragePointSeparation
    displacements = 'disp_z'
    first_point = '14.9 -14.9 32.542'# (21.30 -45)
    last_point =  '14.9 -14.9 33.042'# (21.30 -45)
  [../]

  [./cyl_gnd_shade_rx0]
    type = PointValue
    variable = disp_x
    point = '14.713 14.713 10.596'# (20.80 45)
  [../]
  [./cyl_gnd_shade_rx1]
    type = PointValue
    variable = disp_x
    point = '14.9 14.9 10.596'# (21.30 45)
  [../]
  [./cyl_gnd_shade_ry0]
    type = PointValue
    variable = disp_y
    point = '14.713 14.713 10.596'# (20.80 45)
  [../]
  [./cyl_gnd_shade_ry1]
    type = PointValue
    variable = disp_y
    point = '14.9 14.9 10.596'# (21.30 45)
  [../]
  [./cyl_gnd_shade_thx0]
    type = PointValue
    variable = disp_x
    point = '16.321 13.600 10.596'# (21.30 40)
  [../]
  [./cyl_gnd_shade_thx1]
    type = PointValue
    variable = disp_x
    point = '13.600 16.321 10.596'# (21.30 50)
  [../]
  [./cyl_gnd_shade_thy0]
    type = PointValue
    variable = disp_y
    point = '16.321 13.600 10.596'# (21.30 40)
  [../]
  [./cyl_gnd_shade_thy1]
    type = PointValue
    variable = disp_y
    point = '13.600 16.321 10.596'# (21.30 50)
  [../]
  [./cyl_gnd_shade_z]
    type = AveragePointSeparation
    displacements = 'disp_z'
    first_point = '14.9 14.9 10.346'# (21.30 45)
    last_point =  '14.9 14.9 10.846'# (21.30 45)
  [../]

  [./cyl_gnd_sun_rx0]
    type = PointValue
    variable = disp_x
    point = '14.713 -14.713 10.596'# (20.80 -45)
  [../]
  [./cyl_gnd_sun_rx1]
    type = PointValue
    variable = disp_x
    point = '14.9 -14.9 10.596'# (21.30 -45)
  [../]
  [./cyl_gnd_sun_ry0]
    type = PointValue
    variable = disp_y
    point = '14.713 -14.713 10.596'# (20.80 -45)
  [../]
  [./cyl_gnd_sun_ry1]
    type = PointValue
    variable = disp_y
    point = '14.9 -14.9 10.596'# (21.30 -45)
  [../]
  [./cyl_gnd_sun_thx0]
    type = PointValue
    variable = disp_x
    point = '16.321 -13.600 10.596'# (21.30 -40)
  [../]
  [./cyl_gnd_sun_thx1]
    type = PointValue
    variable = disp_x
    point = '13.600 -16.321 10.596'# (21.30 -50)
  [../]
  [./cyl_gnd_sun_thy0]
    type = PointValue
    variable = disp_y
    point = '16.321 -13.600 10.596'# (21.30 -40)
  [../]
  [./cyl_gnd_sun_thy1]
    type = PointValue
    variable = disp_y
    point = '13.600 -16.321 10.596'# (21.30 -50)
  [../]
  [./cyl_gnd_sun_z]
    type = AveragePointSeparation
    displacements = 'disp_z'
    first_point = '14.9 -14.9 10.346'# (21.30 -45)
    last_point =  '14.9 -14.9 10.846'# (21.30 -45)
  [../]

  [./dome_shade_rx0]
    type = PointValue
    variable = disp_x
    point = '10.345 10.345 58.503'# (20.675 45, 45) z=43.89
  [../]
  [./dome_shade_rx1]
    type = PointValue
    variable = disp_x
    point = '10.400 10.400 58.857'# (21.175 45, 45) z=43.89
  [../]
  [./dome_shade_ry0]
    type = PointValue
    variable = disp_y
    point = '10.345 10.345 58.503'# (20.675 45, 45) z=43.89
  [../]
  [./dome_shade_ry1]
    type = PointValue
    variable = disp_y
    point = '10.400 10.400 58.857'# (21.175 45, 45) z=43.89
  [../]
  [./dome_shade_rz0]
    type = PointValue
    variable = disp_z
    point = '10.345 10.345 58.503'# (20.675 45, 45) z=43.89
  [../]
  [./dome_shade_rz1]
    type = PointValue
    variable = disp_z
    point = '10.400 10.400 58.857'# (21.175 45, 45) z=43.89
  [../]
  [./dome_shade_thx0]
    type = PointValue
    variable = disp_x
    point = '10.900  9.633 58.857'# (21.175 45, 40) z=43.89
  [../]
  [./dome_shade_thx1]
    type = PointValue
    variable = disp_x
    point = '9.633  10.900 58.857'# (21.175 45, 50) z=43.89
  [../]
  [./dome_shade_thy0]
    type = PointValue
    variable = disp_y
    point = '10.900  9.633 58.857'# (21.175 45, 40) z=43.89
  [../]
  [./dome_shade_thy1]
    type = PointValue
    variable = disp_y
    point = '9.633  10.900 58.857'# (21.175 45, 50) z=43.89
  [../]
  [./dome_shade_phix0]
    type = PointValue
    variable = disp_x
    point = '9.300 9.300 60.104'# (21.175 40, 45) z=43.89
  [../]
  [./dome_shade_phix1]
    type = PointValue
    variable = disp_x
    point = '10.500   10.500 57.495'# (21.175 50, 45) z=43.89
  [../]
  [./dome_shade_phiy0]
    type = PointValue
    variable = disp_y
    point = '9.300 9.300 60.104'# (21.175 40, 45) z=43.89
  [../]
  [./dome_shade_phiy1]
    type = PointValue
    variable = disp_y
    point = '10.500   10.500 57.495'# (21.175 50, 45) z=43.89
  [../]
  [./dome_shade_phiz0]
    type = PointValue
    variable = disp_z
    point = '9.300 9.300 60.104'# (21.175 40, 45) z=43.89
  [../]
  [./dome_shade_phiz1]
    type = PointValue
    variable = disp_z
    point = '10.500   10.500 57.495'# (21.175 50, 45) z=43.89
  [../]

  [./dome_sun_rx0]
    type = PointValue
    variable = disp_x
    point = '10.345 -10.345 58.503'# (20.675 -45, 45) z=43.89
  [../]
  [./dome_sun_rx1]
    type = PointValue
    variable = disp_x
    point = '10.400 -10.400 58.857'# (21.175 -45, 45) z=43.89
  [../]
  [./dome_sun_ry0]
    type = PointValue
    variable = disp_y
    point = '10.345 -10.345 58.503'# (20.675 -45, 45) z=43.89
  [../]
  [./dome_sun_ry1]
    type = PointValue
    variable = disp_y
    point = '10.400 -10.400 58.857'# (21.175 -45, 45) z=43.89
  [../]
  [./dome_sun_rz0]
    type = PointValue
    variable = disp_z
    point = '10.345 -10.345 58.503'# (20.675 -45, 45) z=43.89
  [../]
  [./dome_sun_rz1]
    type = PointValue
    variable = disp_z
    point = '10.400 -10.400 58.857'# (21.175 -45, 45) z=43.89
  [../]
  [./dome_sun_thx0]
    type = PointValue
    variable = disp_x
    point = '10.900  -9.633 58.857'# (21.175 -45, 40) z=43.89
  [../]
  [./dome_sun_thx1]
    type = PointValue
    variable = disp_x
    point = '9.633  -10.900 58.857'# (21.175 -45, 50) z=43.89
  [../]
  [./dome_sun_thy0]
    type = PointValue
    variable = disp_y
    point = '10.900  -9.633 58.857'# (21.175 -45, 40) z=43.89
  [../]
  [./dome_sun_thy1]
    type = PointValue
    variable = disp_y
    point = '9.633  -10.900 58.857'# (21.175 -45, 50) z=43.89
  [../]
  [./dome_sun_phix0]
    type = PointValue
    variable = disp_x
    point = '9.300 -9.300 60.104'# (21.175 -40, 45) z=43.89
  [../]
  [./dome_sun_phix1]
    type = PointValue
    variable = disp_x
    point = '10.500   -10.500 57.495'# (21.175 -50, 45) z=43.89
  [../]
  [./dome_sun_phiy0]
    type = PointValue
    variable = disp_y
    point = '9.300 -9.300 60.104'# (21.175 -40, 45) z=43.89
  [../]
  [./dome_sun_phiy1]
    type = PointValue
    variable = disp_y
    point = '10.500   -10.500 57.495'# (21.175 -50, 45) z=43.89
  [../]
  [./dome_sun_phiz0]
    type = PointValue
    variable = disp_z
    point = '9.300 -9.300 60.104'# (21.175 -40, 45) z=43.89
  [../]
  [./dome_sun_phiz1]
    type = PointValue
    variable = disp_z
    point = '10.500   -10.500 57.495'# (21.175 -50, 45) z=43.89
  [../]

  [./base_bot_shade_x]
    type = PointValue
    variable = disp_x
    point = '16 16 -2.89'# (22.86 45)
  [../]
  [./base_bot_shade_y]
    type = PointValue
    variable = disp_x
    point = '16 16 -2.89'# (22.86 45)
  [../]
  [./base_bot_shade_z]
    type = PointValue
    variable = disp_x
    point = '16 16 -2.89'# (22.86 45)
  [../]
  [./base_bot_sun_x]
    type = PointValue
    variable = disp_x
    point = '16 -16 -2.89'# (22.86 45)
  [../]
  [./base_bot_sun_y]
    type = PointValue
    variable = disp_x
    point = '16 -16 -2.89'# (22.86 45)
  [../]
  [./base_bot_sun_z]
    type = PointValue
    variable = disp_x
    point = '16 -16 -2.89'# (22.86 45)
  [../]

  [./basemat_top_shade_x]
    type = PointValue
    variable = disp_x
    point = '16 16 -2.90' # (37.49 45)
  [../]
  [./basemat_top_shade_y]
    type = PointValue
    variable = disp_y
    point = '16 16 -2.90' # (37.49 45)
  [../]
  [./basemat_top_shade_z]
    type = PointValue
    variable = disp_z
    point = '16 16 -2.90' # (37.49 45)
  [../]
  [./basemat_top_sun_x]
    type = PointValue
    variable = disp_x
    point = '16 -16 -2.90' # (37.49 45)
  [../]
  [./basemat_top_sun_y]
    type = PointValue
    variable = disp_y
    point = '16 -16 -2.90' # (37.49 45)
  [../]
  [./basemat_top_sun_z]
    type = PointValue
    variable = disp_z
    point = '16 -16 -2.90' # (37.49 45)
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Dampers]
  [limitx] #nonlinear iteration
    type = MaxIncrement
    max_increment = 1
    variable = disp_x
  []
  [limity]
    type = MaxIncrement
    max_increment = 1
    variable = disp_y
  []
  [limitz]
    type = MaxIncrement
    max_increment = 1
    variable = disp_z
  []
[]

[Executioner]
  type = Transient
  start_time = 1209600 # 28 days
  dt = 604800 # 259200 # 3 day
  automatic_scaling = true
  end_time = 6307200000 # 7300 days

  solve_type = 'PJFNK'
  # petsc_options_iname = '-pc_type'
  # petsc_options_value = 'lu'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_gmres_restart'
  petsc_options_value = 'lu NONZERO   0.1 50'
  nl_max_its = 20
  l_max_its = 30
  nl_abs_tol = 5e-3
  nl_rel_tol = 1e-5
  line_search = none
  petsc_options = '-snes_converged_reason'
[]

[Outputs]
  perf_graph     = true
  csv = true
  [./Console]
    type = Console
  [../]
  # [./Exo]
  #   type = Exodus
  #   elemental_as_nodal = true
  # [../]
[]

[Debug]
  show_var_residual_norms = true
[]
