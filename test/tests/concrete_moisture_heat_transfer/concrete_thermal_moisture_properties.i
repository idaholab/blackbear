[Mesh]
  [square]
    type = GeneratedMeshGenerator
    dim = 2
  []
[]

[Problem]
  solve = false
[]

[AuxVariables]
  [temperature]
    order = CONSTANT
    family = MONOMIAL
    initial_condition = 20
  []
  [rh]
    order = CONSTANT
    family = MONOMIAL
    initial_condition = 1
  []
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
[]

[AuxKernels]
  [temperature]
    type = FunctionAux
    function = temperature
    variable = temperature
    execute_on = 'initial timestep_begin'
  []
  [rh]
    type = FunctionAux
    function = rh
    variable = rh
    execute_on = 'initial timestep_begin'
  []
  [thermal_conductivity]
    type = MaterialRealAux
    variable = thermal_conductivity
    property = thermal_conductivity
    execute_on = 'initial timestep_end'
  []
  [thermal_capacity]
    type = MaterialRealAux
    variable = thermal_capacity
    property = thermal_capacity
    execute_on = 'initial timestep_end'
  []
  [moisture_capacity]
    type = MaterialRealAux
    variable = moisture_capacity
    property = moisture_capacity
    execute_on = 'initial timestep_end'
  []
  [humidity_diffusivity]
    type = MaterialRealAux
    variable = humidity_diffusivity
    property = humidity_diffusivity
    execute_on = 'initial timestep_end'
  []
[]

[Functions]
  [temperature]
    type = PiecewiseLinear
    xy_data = '20 20
               635 635'
  []
  [rh]
    type = PiecewiseLinear
    xy_data = '0 1
               635 1'
  []
[]

[Materials]
  [concrete]
    type = ConcreteThermalMoisture
    # setup thermal property models and parameters
    # options available: CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004 KIM-2003
    thermal_conductivity_model =  KODUR-2004
    thermal_capacity_model     =  KODUR-2004
    aggregate_type = Siliceous               #options: Siliceous Carbonate

    ref_density_of_concrete = 2231.0         # in kg/m^3
#    ref_specific_heat_of_concrete = 1100.0   # in J/(Kg.0C)
#    ref_thermal_conductivity_of_concrete = 3 # in W/(m.0C)


    # setup moisture capacity and humidity diffusivity models
    aggregate_pore_type = dense              #options: dense porous
    aggregate_mass = 1877.0                  #mass of aggregate (kg) per m^3 of concrete
    cement_type = 1                          #options: 1 2 3 4
    cement_mass = 354.0                      #mass of cement (kg) per m^3 of concrete
    water_to_cement_ratio       = 0.43
    concrete_cure_time          = 23.0       #curing time in (days)

    # options available for humidity diffusivity:
    moisture_diffusivity_model = Bazant      #options: Bazant Xi Mensi
    D1 = 3.0e-10

    coupled_moisture_diffusivity_factor = 1.0e-3  # factor for mositure diffusivity due to heat

    # coupled nonlinear variables
    relative_humidity = rh
    temperature = temperature
  []
[]

[Postprocessors]
  [temperature]
    type = ElementAverageValue
    variable = temperature
    execute_on = 'initial timestep_end'
  []
  [rh]
    type = ElementAverageValue
    variable = rh
    execute_on = 'initial timestep_end'
  []
  [thermal_conductivity]
    type = ElementAverageValue
    variable = thermal_conductivity
    execute_on = 'initial timestep_end'
  []
  [thermal_capacity]
    type = ElementAverageValue
    variable = thermal_capacity
    execute_on = 'initial timestep_end'
  []
  [moisture_capacity]
    type = ElementAverageValue
    variable = moisture_capacity
    execute_on = 'initial timestep_end'
  []
  [humidity_diffusivity]
    type = ElementAverageValue
    variable = humidity_diffusivity
    execute_on = 'initial timestep_end'
  []
[]

[Executioner]
  type = Transient
  start_time = 20.0
  end_time = 635.0
  dt = 1.0
[]

[Outputs]
  csv = true
[]
