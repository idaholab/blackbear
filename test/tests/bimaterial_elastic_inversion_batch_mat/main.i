[Optimization]
[]

[OptimizationReporter]
  type = OptimizationReporter
  parameter_names = 'lambda mu'
  num_values = '3 3'
  #initial_condition = '5 4 3 ; 1 2 1'
  initial_condition = '5.0 5.0 5.0; 1.0 1.0 1.0'
  lower_bounds = '0.1; 0.1'
  upper_bounds = '10.0; 10.0'
  measurement_points = '-1.0 -1.0 0.0
                        -1.0  0.0 0.0
                        -1.0  1.0 0.0
                         0.0 -1.0 0.0
                         0.0  0.0 0.0
                         0.0  1.0 0.0
                         1.0 -1.0 0.0
                         1.0  0.0 0.0
                         1.0  1.0 0.0'
  measurement_values = '4.464620e+00 6.447155e+00 8.434803e+00
                        4.176264e+00 6.172984e+00 8.200859e+00
                        4.049477e+00 6.052499e+00 8.079385e+00'
[]

[Executioner]
  type = Optimize
  tao_solver = taobqnls #taobncg #taoblmvm
  petsc_options_iname = '-tao_gatol -tao_ls_type -tao_max_it'
  petsc_options_value = '1e-10 unit 1000'

  #THESE OPTIONS ARE FOR TESTING THE ADJOINT GRADIENT
  #petsc_options_iname='-tao_max_it -tao_fd_test -tao_test_gradient -tao_fd_gradient -tao_fd_delta -tao_gatol'
  #petsc_options_value='1 true true false 1e-8 0.1'
  #petsc_options = '-tao_test_gradient_view'
  verbose = true
[]

[MultiApps]
  [forward]
    type = FullSolveMultiApp
    input_files = model.i
    execute_on = FORWARD
  []
  [adjoint]
    type = FullSolveMultiApp
    input_files = grad.i
    execute_on = ADJOINT
  []
[]

[Transfers]
  [toForward]
    type = MultiAppReporterTransfer
    to_multi_app = forward
    from_reporters = 'OptimizationReporter/measurement_xcoord
                      OptimizationReporter/measurement_ycoord
                      OptimizationReporter/measurement_zcoord
                      OptimizationReporter/measurement_time
                      OptimizationReporter/measurement_values
                      OptimizationReporter/lambda
                      OptimizationReporter/mu'
    to_reporters = 'measure_data/measurement_xcoord
                    measure_data/measurement_ycoord
                    measure_data/measurement_zcoord
                    measure_data/measurement_time
                    measure_data/measurement_values
                    parametrization/lambda
                    parametrization/mu'
  []
  [get_misfit]
    type = MultiAppReporterTransfer
    from_multi_app = forward
    from_reporters = 'measure_data/simulation_values'
    to_reporters = 'OptimizationReporter/simulation_values'
  []
  [set_state_for_adjoint]
    type = MultiAppCopyTransfer
    from_multi_app = forward
    to_multi_app = adjoint
    source_variable = 'ux uy'
    variable = 'state_x state_y'
  []
  [setup_adjoint_run]
    type = MultiAppReporterTransfer
    to_multi_app = adjoint
    from_reporters = 'OptimizationReporter/measurement_xcoord
                      OptimizationReporter/measurement_ycoord
                      OptimizationReporter/measurement_zcoord
                      OptimizationReporter/measurement_time
                      OptimizationReporter/misfit_values
                      OptimizationReporter/lambda
                      OptimizationReporter/mu'
    to_reporters = 'misfit/measurement_xcoord
                    misfit/measurement_ycoord
                    misfit/measurement_zcoord
                    misfit/measurement_time
                    misfit/misfit_values
                    parametrization/lambda
                    parametrization/mu'
  []
  [get_grad_lambda]
    type = MultiAppReporterTransfer
    from_multi_app = adjoint
    from_reporters = 'grad_lambda/inner_product'
    to_reporters = 'OptimizationReporter/grad_lambda'
  []
  [get_grad_mu]
    type = MultiAppReporterTransfer
    from_multi_app = adjoint
    from_reporters = 'grad_mu/inner_product'
    to_reporters = 'OptimizationReporter/grad_mu'
  []
[]

[Reporters]
  [optInfo]
    type = OptimizationInfo
    items = 'current_iterate function_value gnorm'
  []
[]

[Outputs]
  console = false
  csv = true
[]
