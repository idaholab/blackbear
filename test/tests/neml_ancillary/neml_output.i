# Simple 3D test

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  [msh]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 1
    ny = 1
    nz = 1
  []
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = SMALL
    add_variables = true
    new_system = true
    formulation = TOTAL
    volumetric_locking_correction = true
    generate_output = 'cauchy_stress_xx cauchy_stress_yy cauchy_stress_zz cauchy_stress_xy '
                      'cauchy_stress_xz cauchy_stress_yz mechanical_strain_xx mechanical_strain_yy mechanical_strain_zz mechanical_strain_xy '
                      'mechanical_strain_xz mechanical_strain_yz'
  []
[]

[Functions]
  [pullx]
    type = ParsedFunction
    value = '0.1*t'
  []
[]

[AuxVariables]
  [eps]
    family = MONOMIAL
    order = CONSTANT
  []
[]

[AuxKernels]
  [eps]
    type = NEMLStateAux
    database = "examples.xml"
    model = "complex_example"
    state_variable = "alpha"
    variable = eps
  []
[]

[BCs]
  [leftx]
    type = DirichletBC
    preset = true
    boundary = left
    variable = disp_x
    value = 0.0
  []
  [lefty]
    type = DirichletBC
    preset = true
    boundary = left
    variable = disp_y
    value = 0.0
  []
  [leftz]
    type = DirichletBC
    preset = true
    boundary = left
    variable = disp_z
    value = 0.0
  []
  [pull_x]
    type = FunctionDirichletBC
    boundary = right
    variable = disp_x
    function = pullx
  []
[]

[Materials]
  [stress]
    type = CauchyStressFromNEML
    database = "examples.xml"
    model = "complex_example"
    large_kinematics = false
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [stress]
    type = ElementAverageValue
    variable = cauchy_stress_xx
  []
  [strain]
    type = ElementAverageValue
    variable = mechanical_strain_xx
  []
  [eps]
    type = ElementAverageValue
    variable = eps
  []
[]

[Executioner]
  type = Transient

  solve_type = 'newton'
  line_search = none

  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  l_max_its = 2
  l_tol = 1e-14
  nl_max_its = 15
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10

  start_time = 0.0
  dt = 0.1
  dtmin = 0.1
  end_time = 1
[]

[Outputs]
  csv = true
[]
