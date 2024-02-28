# This is a common input file that can be used for all NEML regression tests, which
# use a material driver.
# Notes:
# 1) A single csv file provides both the expected temperature and strain inputs and
#    stress outputs. This file is almost identical to the one used in the NEML tests,
#    is named 'test_x_out.csv', and lives in the 'gold' directory. The only change
#    made to this file is that a first row with headings is added. It may seem odd
#    to use the same file for the inputs and the gold results, but it facilitates
#    supplying and comparing to the NEML regression tests.
# 2) Mandel notation is used in the ordering of the stress/strain components in this
#    file. There is a scaling factor of sqrt(2) used in converting between those and
#    the tensor components used in MOOSE.
# 3) Because all DOFs on the nodes of the single element are fully prescribed, there
#    is no solution needed, so every step solves with no iterations.
# 4) The computations are a bit slow will full integration. Single point integration
#    can be used here without changing the solution, and is employed to accelerate
#    these computations.

# This version uses the Lagrangian kernels and the corresponding NEML driver

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  [cube]
    type = GeneratedMeshGenerator
    dim = 3
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    new_system = true
    formulation = TOTAL
    strain = SMALL
    generate_output = 'strain_xx strain_yy strain_zz strain_xy strain_yz strain_xz
                       cauchy_stress_xx cauchy_stress_yy cauchy_stress_zz cauchy_stress_xy cauchy_stress_yz cauchy_stress_xz'
    add_variables = true
  []
[]

[AuxVariables]
  [temperature]
  []
[]

[AuxKernels]
  [temperature]
    type = FunctionAux
    function = temperature
    variable = temperature
    execute_on = 'initial timestep_begin'
  []
[]


[BCs]
  [x]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = 'left right back front bottom top'
    function = u_x
  []
  [y]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = 'left right back front bottom top'
    function = u_y
  []
  [z]
    type = FunctionDirichletBC
    variable = disp_z
    boundary = 'left right back front bottom top'
    function = u_z
  []
[]

[Functions]
  [temperature]
    type = PiecewiseLinear
    format = columns
    xy_in_file_only = false
    x_index_in_file = 0
    y_index_in_file = 1
  []
  [strain_xx]
    type = PiecewiseLinear
    format = columns
    xy_in_file_only = false
    x_index_in_file = 0
    y_index_in_file = 2
  []
  [strain_yy]
    type = PiecewiseLinear
    format = columns
    xy_in_file_only = false
    x_index_in_file = 0
    y_index_in_file = 3
  []
  [strain_zz]
    type = PiecewiseLinear
    format = columns
    xy_in_file_only = false
    x_index_in_file = 0
    y_index_in_file = 4
  []
  [strain_yz]
    type = PiecewiseLinear
    format = columns
    xy_in_file_only = false
    x_index_in_file = 0
    y_index_in_file = 5
  []
  [strain_xz]
    type = PiecewiseLinear
    format = columns
    xy_in_file_only = false
    x_index_in_file = 0
    y_index_in_file = 6
  []
  [strain_xy]
    type = PiecewiseLinear
    format = columns
    xy_in_file_only = false
    x_index_in_file = 0
    y_index_in_file = 7
  []
  [u_x]
    type = ParsedFunction
    symbol_names = 'strain_xx strain_xy strain_xz'
    symbol_values = 'strain_xx strain_xy strain_xz'
    expression = 'x*strain_xx + 1/sqrt(2)*y*strain_xy + 1/sqrt(2)*z*strain_xz'
  []
  [u_y]
    type = ParsedFunction
    symbol_names = 'strain_yy strain_xy strain_yz'
    symbol_values = 'strain_yy strain_xy strain_yz'
    expression = 'y*strain_yy + 1/sqrt(2)*x*strain_xy + 1/sqrt(2)*z*strain_yz'
  []
  [u_z]
    type = ParsedFunction
    symbol_names = 'strain_zz strain_xz strain_yz'
    symbol_values = 'strain_zz strain_xz strain_yz'
    expression = 'z*strain_zz + 1/sqrt(2)*x*strain_xz + 1/sqrt(2)*y*strain_yz'
  []
[]

[Materials]
  [stress]
    type = CauchyStressFromNEML
    model = 'model'
    temperature = temperature
    large_kinematics = false
  []
[]

[Postprocessors]
  [stress_11]
    type = ElementAverageValue
    variable = cauchy_stress_xx
    execute_on = 'initial timestep_end'
  []
  [stress_22]
    type = ElementAverageValue
    variable = cauchy_stress_yy
    execute_on = 'initial timestep_end'
  []
  [stress_33]
    type = ElementAverageValue
    variable = cauchy_stress_zz
    execute_on = 'initial timestep_end'
  []
  [stress_12_tensor]
    type = ElementAverageValue
    variable = cauchy_stress_xy
    execute_on = 'initial timestep_end'
    outputs = none
  []
  [stress_12]
    type = LinearCombinationPostprocessor
    pp_names = 'stress_12_tensor'
    pp_coefs = '1.41421356237'
    execute_on = 'initial timestep_end'
  []
  [stress_13_tensor]
    type = ElementAverageValue
    variable = cauchy_stress_xz
    execute_on = 'initial timestep_end'
    outputs = none
  []
  [stress_13]
    type = LinearCombinationPostprocessor
    pp_names = 'stress_13_tensor'
    pp_coefs = '1.41421356237'
    execute_on = 'initial timestep_end'
  []
  [stress_23_tensor]
    type = ElementAverageValue
    variable = cauchy_stress_yz
    execute_on = 'initial timestep_end'
    outputs = none
  []
  [stress_23]
    type = LinearCombinationPostprocessor
    pp_names = 'stress_23_tensor'
    pp_coefs = '1.41421356237'
    execute_on = 'initial timestep_end'
  []
  [strain_11]
    type = ElementAverageValue
    variable = strain_xx
    execute_on = 'initial timestep_end'
  []
  [strain_22]
    type = ElementAverageValue
    variable = strain_yy
    execute_on = 'initial timestep_end'
  []
  [strain_33]
    type = ElementAverageValue
    variable = strain_zz
    execute_on = 'initial timestep_end'
  []
  [strain_12_tensor]
    type = ElementAverageValue
    variable = strain_xy
    execute_on = 'initial timestep_end'
    outputs = none
  []
  [strain_12]
    type = LinearCombinationPostprocessor
    pp_names = 'strain_12_tensor'
    pp_coefs = '1.41421356237'
    execute_on = 'initial timestep_end'
  []
  [strain_13_tensor]
    type = ElementAverageValue
    variable = strain_xz
    execute_on = 'initial timestep_end'
    outputs = none
  []
  [strain_13]
    type = LinearCombinationPostprocessor
    pp_names = 'strain_13_tensor'
    pp_coefs = '1.41421356237'
    execute_on = 'initial timestep_end'
  []
  [strain_23_tensor]
    type = ElementAverageValue
    variable = strain_yz
    execute_on = 'initial timestep_end'
    outputs = none
  []
  [strain_23]
    type = LinearCombinationPostprocessor
    pp_names = 'strain_23_tensor'
    pp_coefs = '1.41421356237'
    execute_on = 'initial timestep_end'
  []
  [temperature]
    type = ElementAverageValue
    variable = temperature
    execute_on = 'initial timestep_end'
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  [Quadrature]
    order = FIRST #Single qp to make this fully prescribed, single-element test run faster
  []

  l_max_its = 5
  l_tol = 1e-14
  nl_max_its = 10
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-12
  nl_forced_its = 2
  line_search = none

  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  end_time = 1000.0
  dt = 10.0
[]

[Outputs]
  [csv]
    type = CSV
  []
  exodus = true
[]
