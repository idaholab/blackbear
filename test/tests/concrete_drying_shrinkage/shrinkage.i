[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 1
  elem_type = HEX8
[]

[Functions]
  [./h_function]
    type = PiecewiseLinear
    x = '0 10 20'
    y = '1 0 1'
  [../]
[]

[AuxVariables]
  [./h]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Physics]
  [SolidMechanics]
    [QuasiStatic]
      [all]
        strain = SMALL
        add_variables = true
        generate_output = 'stress_xx stress_yy stress_zz strain_xx strain_yy strain_zz'
        eigenstrain_names = 'shrinkage'
      []
    []
  []
[]

[AuxKernels]
  [./humidity]
    type = FunctionAux
    variable = h
    function = h_function
  [../]
[]

[BCs]
  [./symmy]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  [../]
  [./symmx]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0
  [../]
  [./symmz]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0
  [../]
[]

[Materials]
  [./stress]
    type = ComputeLinearElasticStress
  [../]
  [./concrete]
    type = ComputeIsotropicElasticityTensor
    poissons_ratio = 0.2
    youngs_modulus = 30e9
  [../]
  [./shrinkage]
    type = ConcreteDryingShrinkage
    eigenstrain_name = shrinkage
    humidity = h
    drying_shrinkage_coefficient = -0.3e-3
    exponent = 1
  [../]
[]

[Postprocessors]
  [./stress_xx]
    type = ElementAverageValue
    variable = stress_xx
    block = 'ANY_BLOCK_ID 0'
  [../]
  [./stress_yy]
    type = ElementAverageValue
    variable = stress_yy
    block = 'ANY_BLOCK_ID 0'
  [../]
  [./stress_zz]
    type = ElementAverageValue
    variable = stress_zz
    block = 'ANY_BLOCK_ID 0'
  [../]
  [./strain_xx]
    type = ElementAverageValue
    variable = strain_xx
    block = 'ANY_BLOCK_ID 0'
  [../]
  [./strain_yy]
    type = ElementAverageValue
    variable = strain_yy
    block = 'ANY_BLOCK_ID 0'
  [../]
  [./strain_zz]
    type = ElementAverageValue
    variable = strain_zz
    block = 'ANY_BLOCK_ID 0'
  [../]
  [./humidity]
    type = ElementAverageValue
    variable = h
    block = 'ANY_BLOCK_ID 0'
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'

  l_max_its  = 50
  l_tol      = 1e-6
  nl_max_its = 20
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-10

  end_time = 20
  dt = 1
[]

[Outputs]
  file_base = shrinkage_out
  exodus = true
[]
