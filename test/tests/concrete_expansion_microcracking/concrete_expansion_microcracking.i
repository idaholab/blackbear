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


[AuxVariables]
  [./expansion_vstrain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./expansion_strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./expansion_strain_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./expansion_strain_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./microcracking]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./damage_index]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Functions]
  [./expansion_vs_time]
    type = ParsedFunction
    expression = 'max(0, t - 0.0001)'
  [../]
[]

[Physics]
  [SolidMechanics]
    [QuasiStatic]
      [all]
        strain = SMALL
        incremental = true
        add_variables = true
        generate_output = 'stress_xx stress_yy stress_zz strain_xx strain_yy strain_zz'
        eigenstrain_names = 'concrete_expansion'
      []
    []
  []
[]


[AuxKernels]
  [./expansion_vstrain]
    type = MaterialRealAux
    variable = expansion_vstrain
    property = test_volumetric_strain
    execute_on = 'timestep_end'
  [../]
  [./expansion_strain_xx]
    type = RankTwoAux
    rank_two_tensor = concrete_expansion
    variable = expansion_strain_xx
    index_i = 0
    index_j = 0
    execute_on = 'timestep_end'
  [../]
  [./expansion_strain_yy]
    type = RankTwoAux
    rank_two_tensor = concrete_expansion
    variable = expansion_strain_yy
    index_i = 1
    index_j = 1
    execute_on = 'timestep_end'
  [../]
  [./expansion_strain_zz]
    type = RankTwoAux
    rank_two_tensor = concrete_expansion
    variable = expansion_strain_zz
    index_i = 2
    index_j = 2
    execute_on = 'timestep_end'
  [../]
  [./damage_index]
    type = MaterialRealAux
    variable = damage_index
    property = damage_index
    execute_on = 'timestep_end'
  [../]
[]

[BCs]
  [./symmx]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0
  [../]
  [./symmy]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  [../]
  [./symmz]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0
  [../]

  [./load_x]
    type = NeumannBC
    variable = disp_x
    boundary = right
    value    = 0
  [../]
  [./load_y]
    type = NeumannBC
    variable = disp_y
    boundary = top
    value    = 0
  [../]
  [./load_z]
    type = NeumannBC
    variable = disp_z
    boundary = front
    value    = 0
  [../]
[]

[Materials]
  [microcracking]
    type = ConcreteExpansionMicrocrackingDamage
    microcracking_eigenstrain_name = concrete_expansion
    microcracking_initiation_strain = 0.0001
    microcracking_strain_branch = 0.0002
  []

  [stress]
    type = ComputeDamageStress
    damage_model = microcracking
  []

  [./concrete]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 30e9
    poissons_ratio = 0.2
  [../]

  [concrete_expansion]
    type = TestConcreteExpansionEigenstrain
    expansion_type = Isotropic
    function = expansion_vs_time
    eigenstrain_name = concrete_expansion
  []
[]

[Postprocessors]
  [./expansion_vol_strain]
    type = ElementAverageValue
    variable = expansion_vstrain
  [../]
  [./expansion_strain_xx]
    type = ElementAverageValue
    variable = expansion_strain_xx
  [../]
  [./stress_xx]
    type = ElementAverageValue
    variable = stress_xx
  [../]
  [./strain_xx]
    type = ElementAverageValue
    variable = strain_xx
  [../]
  [./expansion_strain_yy]
    type = ElementAverageValue
    variable = expansion_strain_yy
  [../]
  [./stress_yy]
    type = ElementAverageValue
    variable = stress_yy
  [../]
  [./strain_yy]
    type = ElementAverageValue
    variable = strain_yy
  [../]
  [./expansion_strain_zz]
    type = ElementAverageValue
    variable = expansion_strain_zz
  [../]
  [./stress_zz]
    type = ElementAverageValue
    variable = stress_zz
  [../]
  [./strain_zz]
    type = ElementAverageValue
    variable = strain_zz
  [../]
  [./damage_index]
    type = ElementAverageValue
    variable = damage_index
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
  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-6

  start_time = 0
  dt = 0.0001
  dtmin = 0.0001
  end_time = 0.0011
[]

[Outputs]
  file_base = concrete_expansion_microcracking_out
  csv = true
[]
