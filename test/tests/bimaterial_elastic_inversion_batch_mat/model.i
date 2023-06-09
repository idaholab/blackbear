[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 11
    ny = 11
    xmin = -4
    xmax = 4
    ymin = -4
    ymax = 4
  []
[]

[Variables]
  [ux]
  []
  [uy]
  []
[]

[AuxVariables]
  [dummy]
  []
[]

[Kernels]
  [div_sigma_x]
    type = StressDivergenceTensors
    variable = ux
    displacements = 'ux uy'
    component = 0
    volumetric_locking_correction = false
  []
  [div_sigma_y]
    type = StressDivergenceTensors
    variable = uy
    displacements = 'ux uy'
    component = 1
    volumetric_locking_correction = false
  []
[]

[BCs]
  [bottom_ux]
    type = DirichletBC
    variable = ux
    boundary = bottom
    value = 0.0
  []
  [bottom_uy]
    type = DirichletBC
    variable = uy
    boundary = bottom
    value = 0.0
  []
  [top_fx]
    type = NeumannBC
    variable = ux
    boundary = top
    value = 1.0
  []
  [top_fy]
    type = NeumannBC
    variable = uy
    boundary = top
    value = 1.0
  []
[]

[Materials]
  [stress]
    type = ComputeLinearElasticStress
  []
  [strain]
    type = ComputeSmallStrain
    displacements = 'ux uy'
  []
  [elasticity_tensor]
     type = ComputeVariableIsotropicElasticityTensor
     args = dummy
     youngs_modulus = E_material
     poissons_ratio = nu_material
  []
  [E_material]
    type = GenericFunctionMaterial
    prop_names = 'E_material'
    prop_values = E
  []
  [nu_material]
    type = GenericFunctionMaterial
    prop_names = 'nu_material'
    prop_values = nu
  []
[]

[Functions]
  [E]
    type = ParsedFunction
    expression = mu*(3*lambda+2*mu)/(lambda+mu)
    symbol_names = 'lambda mu'
    symbol_values = 'lambda mu'
  []
  [nu]
    type = ParsedFunction
    expression = lambda/2/(lambda+mu)
    symbol_names = 'lambda mu'
    symbol_values = 'lambda mu'
  []
  [lambda]
    type = NearestReporterCoordinatesFunction
    x_coord_name = parametrization/coordx
    y_coord_name = parametrization/coordy
    value_name = parametrization/lambda
  []
  [mu]
    type = NearestReporterCoordinatesFunction
    x_coord_name = parametrization/coordx
    y_coord_name = parametrization/coordy
    value_name = parametrization/mu
  []
[]

[Reporters]
  [measure_data]
    type = OptimizationData
    variable = ux
  []
  [parametrization]
    type = ConstantReporter
    real_vector_names = 'coordx coordy lambda mu'
    real_vector_values = '0 1 2; 0 1 2; 5 4 3; 1 2 3'
  []
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
[]

[Postprocessors]
  [point1]
    type = PointValue
    point = '-1.0 -1.0 0.0'
    variable = ux
    execute_on = TIMESTEP_END
  []
  [point2]
    type = PointValue
    point = '-1.0 0.0 0.0'
    variable = ux
    execute_on = TIMESTEP_END
  []
  [point3]
    type = PointValue
    point = '-1.0 1.0 0.0'
    variable = ux
    execute_on = TIMESTEP_END
  []
[]

[Outputs]
  file_base = 'forward'
  console = false
[]
