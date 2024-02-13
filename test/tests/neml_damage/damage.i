[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
  [disp_z]
  []
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  ny = 2
  nz = 2
[]

[Kernels]
  [TensorMechanics]
  []
[]

[AuxVariables]
  [damage_index]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [damage_index]
    type = MaterialRealAux
    variable = damage_index
    property = damage_index
    execute_on = timestep_end
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
    preset = false
  []
[]

[Functions]
  [pull]
    type = PiecewiseLinear
    x = '0 1'
    y = '0 0.15'
  []
[]

[Materials]
  [strain]
    type = ComputeSmallStrain
  []

  [stress]
    type = NEMLStress
    model = test_powerdamage
    database = 'example.xml'
  []
[]

[Preconditioning]
  [pc]
    type = SMP
    full = True
  []
[]

[Postprocessors]
  [damge]
    type = ElementAverageValue
    variable = damage_index
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  line_search = none
  l_max_its = 2
  nl_max_its = 25
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10

  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  end_time = 1.0
  dt = 0.25
[]

[Outputs]
  exodus = true
  console = true
[]
