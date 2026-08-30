[GlobalParams]
  block = '0 1'
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 10
  []
  [solid1]
    type = SubdomainBoundingBoxGenerator
    input = gmg
    block_id = 0
    block_name = solid1
    bottom_left = '0 0 0'
    top_right = '1 1 0'
  []

  [nan]
    input = solid1
    type = SubdomainPerElementGenerator
    element_ids = '0 1 10 11 12 21 22 23 32 33 34 43 44 45 54 55'
    subdomain_ids = '2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2'
  []

  add_subdomain_names = "solid2"
  add_subdomain_ids = 1
  use_displaced_mesh = false
[]

[Problem]
  kernel_coverage_check = FALSE
  material_coverage_check = FALSE
[]

[Variables]
  [T]
    order = FIRST
  []
[]

[UserObjects]
  [extrapolation_patch_T]
    type = NodalPatchRecoveryVariable
    patch_polynomial_order = FIRST
    variable = 'T'
    execute_on = 'TIMESTEP_BEGIN'
  []
[]

[SpatioTemporalHeat]
  path_file = 'gold/path.csv'

  # for path
  verbose = true

  # for esm
  block = '1 2'
  target_subdomain = "solid2"
  radius = 0.1
  execute_on_esm = 'TIMESTEP_BEGIN'

  old_subdomain_reinitialized = false
  reinitialize_subdomains = '1'
  reinitialization_strategy = "POLYNOMIAL_NEIGHBOR"
  reinitialize_variables = "T"
  polynomial_fitters = 'extrapolation_patch_T'

  # for heat source
  power = 1
  a = 0.035
  b = 0.01
  efficiency = 1
  scale = 1

  # for kernel
  heat_variable = T
[]

[Kernels]
  [heat_conduction]
    type = HeatConduction
    variable = T
  []
  [time_derivative]
    type = HeatConductionTimeDerivative
    variable = T
  []
[]

[Materials]
  [thermal]
    type = HeatConductionMaterial
    thermal_conductivity = 45.0
    specific_heat = 0.5
  []
  [density]
    type = GenericConstantMaterial
    prop_names = 'density'
    prop_values = 8000.0
  []
[]

[Executioner]
  type = Transient
  dt = 0.5
  end_time = 10
[]

[Outputs]
  exodus = true
[]
