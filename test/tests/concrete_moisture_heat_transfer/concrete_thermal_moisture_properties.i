Tmin = 28
Tmax = 635

[Mesh]
  type = GeneratedMesh
  dim = 2
[]

[Problem]
  solve = false
[]

[AuxVariables]
  [temperature]
    order = CONSTANT
    family = MONOMIAL
    initial_condition = 28
  []
  [rh]
    order = CONSTANT
    family = MONOMIAL
    initial_condition = 0
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
    xy_data = '28 ${Tmin}
               635 ${Tmax}
               636 ${Tmin}
               1243 ${Tmax}
               1244 ${Tmin}
               1851 ${Tmax}
               1852 ${Tmin}
               2459 ${Tmax}
               2460 ${Tmin}
               3067 ${Tmax}'
  []
  [rh]
    type = PiecewiseLinear
    xy_data = '28 0
               635 0
               636 0.25
               1243 0.25
               1244 0.5
               1851 0.5
               1852 0.75
               2459 0.75
               2460 1.0
               3067 1.0'
  []
[]

[Materials]
  [concrete]
    type = ConcreteThermalMoisture
    # considering default values for concrete mix, thermal and moisture transport models and their parameters
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
  [humidity_diffusivity]
    type = ElementAverageValue
    variable = humidity_diffusivity
    execute_on = 'initial timestep_end'
  []
[]

[Executioner]
  type = Transient
  start_time = 28.0
  end_time = 3067
  dt = 5
[]

[Outputs]
  csv = true
[]
