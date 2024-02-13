[Mesh]
  [circular]
    type = AnnularMeshGenerator
    nr = 5
    nt = 20
    rmin = 0
    rmax = 1
  []
[]

[Variables]
  [u]
    initial_condition = 0.0
  []
[]

[Kernels]
  [Diff]
    type = Diffusion
    variable = u
  []
[]

[BCs]
  [outer]
    type = FunctionOffsetDirichletBC
    variable = u
    boundary = '1'
    function = sol_func
    offset = -0.2
  []
  [inner]
    type = DirichletBC
    variable = u
    value = 0
    boundary = 'rmin'
  []
[]

[Functions]
  [sol_func]
    type = SolutionFunction
    solution = u_sol
    from_variable = u_source
  []
[]

[UserObjects]
  [u_sol]
    type = SolutionUserObject
    mesh = source_circular.e
    system_variables = u_source
  []
[]

[NodalNormals]
  boundary = '1'
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient

  solve_type = 'PJFNK'
  end_time = 1
  dt = 1

  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
