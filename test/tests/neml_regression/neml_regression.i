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
#    file. None of the current tests have nonzero shear stresses or strains, but
#    a conversion between these and the tensorial components will need to be added
#    for tests that have those. We will also need to modify the boundary conditions
#    for shear terms.
# 3) Because all DOFs on the nodes of the single element are fully prescribed, there
#    is no solution needed, so every step solves with no iterations.
# 4) The computations are a bit slow will full integration. Single point integration
#    can be used here without changing the solution, and is employed to accelerate
#    these computations.

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  data_file = 'gold/test1_out.csv'
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
[]

[Modules/TensorMechanics/Master]
  [all]
    generate_output = 'strain_xx strain_yy strain_zz strain_xy strain_yz strain_xz
                       stress_xx stress_yy stress_zz stress_xy stress_yz stress_xz'
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
  [x1]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  []

  [y1]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  []

  [z1]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0.0
  []

  [x2]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = right
    function = strain_xx
  []

  [y2]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = top
    function = strain_yy
  []

  [z2]
    type = FunctionDirichletBC
    variable = disp_z
    boundary = front
    function = strain_zz
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
[]

[Materials]
  [stress]
    type = NEMLStress
    database = test1.xml
    model = 'model'
  []
[]

[Postprocessors]
  [stress_xx]
    type = ElementAverageValue
    variable = stress_xx
    execute_on = 'initial timestep_end'
  []
  [stress_yy]
    type = ElementAverageValue
    variable = stress_yy
    execute_on = 'initial timestep_end'
  []
  [stress_zz]
    type = ElementAverageValue
    variable = stress_zz
    execute_on = 'initial timestep_end'
  []
  [stress_xy]
    type = ElementAverageValue
    variable = stress_xy
    execute_on = 'initial timestep_end'
  []
  [stress_xz]
    type = ElementAverageValue
    variable = stress_xz
    execute_on = 'initial timestep_end'
  []
  [stress_yz]
    type = ElementAverageValue
    variable = stress_yz
    execute_on = 'initial timestep_end'
  []
  [strain_xx]
    type = ElementAverageValue
    variable = strain_xx
    execute_on = 'initial timestep_end'
  []
  [strain_yy]
    type = ElementAverageValue
    variable = strain_yy
    execute_on = 'initial timestep_end'
  []
  [strain_zz]
    type = ElementAverageValue
    variable = strain_zz
    execute_on = 'initial timestep_end'
  []
  [strain_xy]
    type = ElementAverageValue
    variable = strain_xy
    execute_on = 'initial timestep_end'
  []
  [strain_xz]
    type = ElementAverageValue
    variable = strain_xz
    execute_on = 'initial timestep_end'
  []
  [strain_yz]
    type = ElementAverageValue
    variable = strain_yz
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

  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  end_time = 1000.0
  dt = 10.0
[]

[Outputs]
  exodus = false
  [csv]
    type = CSV
    file_base = test1_out
  []
[]
