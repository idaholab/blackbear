[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    add_variables = true
    strain = SMALL
    generate_output = 'strain_xx strain_yy strain_zz strain_xy strain_yz strain_xz stress_xx stress_yy stress_zz stress_xy stress_yz stress_xz'
  []
[]

[BCs]
  [fx]
    type = DirichletBC
    variable = disp_x
    boundary = 4
    value = 0.0
  []

  [fy]
    type = DirichletBC
    variable = disp_y
    boundary = 1
    value = 0.0
  []

  [fz]
    type = DirichletBC
    variable = disp_z
    boundary = 0
    value = 0.0
  []

  [pz]
    type = FunctionDirichletBC
    variable = disp_z
    boundary = 5
    function = pull
  []
[]

[Functions]
  [pull]
    type = PiecewiseLinear
    x = '0 1'
    y = '0 0.1'
  []
[]

[Materials]
  [stress]
    type = NEMLStress
    model = simple_example
    database = 'examples.xml'
  []
[]

[Preconditioning]
  [pc]
    type = SMP
    full = True
  []
[]

[Postprocessors]
  [stress_xx]
    type = ElementAverageValue
    variable = stress_xx
  []
  [stress_yy]
    type = ElementAverageValue
    variable = stress_yy
  []
  [stress_zz]
    type = ElementAverageValue
    variable = stress_zz
  []
  [stress_xy]
    type = ElementAverageValue
    variable = stress_xy
  []
  [stress_xz]
    type = ElementAverageValue
    variable = stress_xz
  []
  [stress_yz]
    type = ElementAverageValue
    variable = stress_yz
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'

  l_max_its = 2
  l_tol = 1e-3
  nl_max_its = 10
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-8

  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  end_time = 1.0
  dt = 1.0
[]

[Outputs]
  exodus = false
  console = true
  csv = true
[]
