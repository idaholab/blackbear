[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  [cube1]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 4
    ny = 4
    nz = 4
    xmax = 0.5
    ymax = 1
    zmax = 1
  []
  [corner_nodes]
    type = ExtraNodesetGenerator
    input = cube1
    new_boundary = '100'
    nodes = '16 17 18 28 38 48 49 58 74 83 99 108 109 114 119 124'
  []
[]

[Modules/TensorMechanics/Master]
  [all]
    add_variables = true
  []
[]

[Functions]
  [sol_func]
    type = SolutionFunction
    solution = disp_sol
    from_variable = disp_x
  []
[]

[BCs]
  [xsymm]
    type = DirichletBC
    boundary = left
    variable = disp_x
    value = 0
  []
  [ysymm]
    type = DirichletBC
    boundary = bottom
    variable = disp_y
    value = 0
  []
  [zsymm]
    type = DirichletBC
    boundary = back
    variable = disp_z
    value = 0
  []
  [load]
    type = FunctionOffsetDirichletBC
    boundary = right
    variable = disp_x
    function = sol_func
    offset = 0.1
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1e9
    poissons_ratio = 0.3
  []
  [stress]
    type = ComputeLinearElasticStress
  []
[]

[UserObjects]
  [disp_sol]
    type = SolutionUserObject
    mesh = source.e
    system_variables = disp_x
  []
[]

[NodalNormals]
  boundary = 'right'
  corner_boundary = 100
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  end_time = 1
  dt = 1
[]

[Outputs]
  exodus = true
/Users/dahaa/projects/blackbear/doc/content/source/bcs/FunctionOffsetDirichletBC.md[]
