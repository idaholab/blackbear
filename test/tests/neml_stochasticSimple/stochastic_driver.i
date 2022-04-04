[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Problem]
  solve = false
  kernel_coverage_check = false
[]

[Distributions]
  [E]
    type = Uniform
    lower_bound = 100000
    upper_bound = 200000
  []
  [nu]
    type = Uniform
    lower_bound = 0.2
    upper_bound = 0.4
  []
[]

[Samplers]
  [neml_values]
    type = MonteCarlo
    num_rows = 5
    distributions = 'E nu'
    execute_on = 'PRE_MULTIAPP_SETUP'
    seed = 1
  []
[]

[MultiApps]
  [sub]
    type = SamplerFullSolveMultiApp
    input_files = 'le_stress_simple.i'
    sampler = neml_values
  []
[]

[Transfers]
  [data0]
    type = SamplerPostprocessorTransfer
    from_multi_app = sub
    sampler = neml_values
    to_vector_postprocessor = avg_stress_zz_collected
    from_postprocessor = "avg_stress_zz"
  []
[]

[Executioner]
  type = Transient
  end_time = 1.0
  dt = 1.0
[]

[VectorPostprocessors]
  [avg_stress_zz_collected]
    type = StochasticResults
    execute_on = 'TIMESTEP_END'
    contains_complete_history = true
  []
  [neml_values_random]
    type = SamplerData
    sampler = neml_values
    execute_on = 'TIMESTEP_END'
    contains_complete_history = true
  []
[]

[Controls]
  [cmdLine]
    type = MultiAppCommandLineControl
    multi_app = sub
    sampler = neml_values

    param_names='Materials/stress/neml_variable_value[0,1]'

  []
[]

[Outputs]
  file_base = out
  exodus = false
  console = true
  csv = true
[]
