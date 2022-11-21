[Problem]
  type = ReferenceResidualProblem
  reference_vector = 'ref'
  extra_tag_vectors = 'ref'
[]

[Mesh]
  file = maqbeth_1d.e
  construct_side_list_from_node_list = true
  coord_type = RZ
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
  [humidity_diffusivity]
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
  [./rh_diff]
    type = MaterialRealAux
    variable = humidity_diffusivity
    property = humidity_diffusivity
    execute_on = 'timestep_end'
  [../]
[]

[Materials]
  [./concrete]
    type = ConcreteThermalMoisture
    block = 1
    # concrete mix proportion parameters
    cement_mass = 354.0 #mass of cement (kg) per m^3 of concrete
    aggregate_mass = 1877.0 #mass of aggregate (kg) per m^3 of concrete
    water_to_cement_ratio = 0.435

    # Thermal transport model and its parameters
    thermal_model = KODUR-2004 #options available: CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004 KIM-2003
    aggregate_type = Siliceous #options: Siliceous Carbonate

    # Moisture transport model and its parameters
    moisture_model = Mensi #options: Bazant Xi Mensi
    A = 1.042e-13
    B = 0.05

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
