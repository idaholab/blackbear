[Problem]
  type = ReferenceResidualProblem
  reference_vector = 'ref'
  extra_tag_vectors = 'ref'
  coord_type = RZ
[]

[Mesh]
  file = maqbeth_1d.e
  construct_side_list_from_node_list = true
[]

[Functions]
  [temp_hist]
    type = PiecewiseLinear
    x = '0.0  36000.0 114120.0 138600.0  213120'
    y = '20.0  20.0      150.0     80.0   200.0'
  []
[]

[Variables]
  [T]
    order = FIRST
    family = LAGRANGE
    initial_condition = 20.0
  []
  [rh]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.96
  []
[]

[AuxVariables]
  [thermal_conductivity]
    order = CONSTANT
    family = Monomial
  []

  [thermal_capacity]
    order = CONSTANT
    family = Monomial
  []

  [moisture_capacity]
    order = CONSTANT
    family = Monomial
  []

  [humidity_diffusivity]
    order = CONSTANT
    family = Monomial
  []

  [water_content]
    order = CONSTANT
    family = Monomial
  []
  [water_hydrated]
    order = CONSTANT
    family = Monomial
  []
[]

[Kernels]
  [T_td]
    type     = ConcreteThermalTimeIntegration
    variable = T
    extra_vector_tags = 'ref'
  []
  [T_diff]
    type     = ConcreteThermalConduction
    variable = T
    extra_vector_tags = 'ref'
  []

  [T_conv]
    type     = ConcreteThermalConvection
    variable = T
    relative_humidity = rh
    extra_vector_tags = 'ref'
  []

  [T_adsorption]
    type     = ConcreteLatentHeat
    variable = T
    H = rh
    extra_vector_tags = 'ref'
  []

  [rh_td]
    type     = ConcreteMoistureTimeIntegration
    variable = rh
    extra_vector_tags = 'ref'
  []

  [h_diff]
    type     = ConcreteMoistureDiffusion
    variable = rh
    temperature = T
    extra_vector_tags = 'ref'
  []
[]

[AuxKernels]
  [k]
    type = MaterialRealAux
    variable = thermal_conductivity
    property = thermal_conductivity
    execute_on = 'timestep_end'
  [../]
  [./capacity]
    type = MaterialRealAux
    variable = thermal_capacity
    property = thermal_capacity
    execute_on = 'timestep_end'
  [../]

  [./rh_capacity]
    type = MaterialRealAux
    variable = moisture_capacity
    property = moisture_capacity
    execute_on = 'timestep_end'
  [../]
  [./rh_diff]
    type = MaterialRealAux
    variable = humidity_diffusivity
    property = humidity_diffusivity
    execute_on = 'timestep_end'
  [../]
  [./wc_diff]
    type = MaterialRealAux
    variable = water_content
    property = moisture_content
    execute_on = 'timestep_end'
  [../]
  [./hydrw_duff]
    type = MaterialRealAux
    variable = water_hydrated
    property = hydrated_water
    execute_on = 'timestep_end'
  [../]
[]

[Materials]
  [./concrete]
    type = ConcreteThermalMoisture
    block = 1
    # options available: CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004 KIM-2003
    thermal_conductivity_model =  KODUR-2004
    thermal_capacity_model     =  KODUR-2004
    aggregate_type = Siliceous               #options: Siliceous Carbonate

    ref_density_of_concrete = 2231.0         # in kg/m^3
    ref_specific_heat_of_concrete = 1100.0   # in J/(Kg.0C)
    ref_thermal_conductivity_of_concrete = 3 # in W/(m.0C)


    # setup moisture capacity and humidity diffusivity models
    aggregate_pore_type = dense              #options: dense porous
    aggregate_mass = 1877.0                  #mass of aggregate (kg) per m^3 of concrete
    cement_type = 1                          #options: 1 2 3 4
    cement_mass = 354.0                      #mass of cement (kg) per m^3 of concrete
    water_to_cement_ratio       = 0.43
    concrete_cure_time          = 23.0       #curing time in (days)

    # options available for humidity diffusivity:
    moisture_diffusivity_model =  Bazant      #options: Bazant Xi Mensi
    D1 = 3.0e-10

    coupled_moisture_diffusivity_factor = 1.0e-3  # factor for mositure diffusivity due to heat

    # coupled nonlinear variables
    relative_humidity = rh
    temperature = T
  [../]
[]

[BCs]
  [./T_left]
    type = FunctionDirichletBC
    variable = T
    boundary = '1'
    function = temp_hist
  [../]

  [./T_right]
    type = ConvectiveFluxBC
    variable = T
    boundary = '2'
    final = 20.0
    rate = 10.0
  [../]

  [./rh_left]
    type = SpecifiedVaporPressureBC
    variable = rh
    boundary = '1'
    duration = 3600
    vapor_pressure = 2500.0
    temperature = T
  [../]
  [./rh_right]
    type = SpecifiedVaporPressureBC
    variable = rh
    boundary = '2'
    duration = 3600
    vapor_pressure = 2500.0
    temperature = T
  [../]
[]

[VectorPostprocessors]
  [profiles]
    type = LineValueSampler
    start_point = '0.5 0 0'
    end_point = '1.1 0 0'
    num_points = 100
    variable = 'T rh'
    sort_by = id
    outputs = csv
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type       = Transient
  solve_type = 'PJFNK'

  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu superlu_dist'

  automatic_scaling = true

  dt = 10000

  end_time = 900000.0

  l_max_its  = 50
  l_tol      = 1e-6
  nl_max_its = 10
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-10

  [Predictor]
    type = SimplePredictor
    scale = 1.0
    skip_times_old = '0.0  36000.0 114120.0 138600.0  213120'
  []
[]

[Outputs]
  exodus     = true
  [csv]
    type = CSV
    file_base = 'csv/out'
    time_data = true
  []
[]
