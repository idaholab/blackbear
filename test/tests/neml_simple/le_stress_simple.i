#
#
#
[GlobalParams]
      displacements = 'disp_x disp_y disp_z'
[]

[Variables]
      [./disp_x]
      [../]
      [./disp_y]
      [../]
      [./disp_z]
      [../]
[]

[Mesh]
      type = GeneratedMesh
      dim = 3
[]

[Kernels]
      [./TensorMechanics]
      [../]
[]

[AuxVariables]
  [./strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_xz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xz]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./strain_xx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_xx
    index_i = 0
    index_j = 0
    execute_on = timestep_end
  [../]
  [./strain_yy]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_yy
    index_i = 1
    index_j = 1
    execute_on = timestep_end
  [../]
  [./strain_zz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_zz
    index_i = 2
    index_j = 2
    execute_on = timestep_end
  [../]
  [./strain_xy]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_xy
    index_i = 0
    index_j = 1
    execute_on = timestep_end
  [../]
  [./strain_yz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_yz
    index_i = 1
    index_j = 2
    execute_on = timestep_end
  [../]
  [./strain_xz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_xz
    index_i = 0
    index_j = 1
    execute_on = timestep_end
  [../]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
    execute_on = timestep_end
  [../]
  [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
    execute_on = timestep_end
  [../]
  [./stress_zz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zz
    index_i = 2
    index_j = 2
    execute_on = timestep_end
  [../]
  [./stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
    index_i = 0
    index_j = 1
    execute_on = timestep_end
  [../]
  [./stress_yz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yz
    index_i = 1
    index_j = 2
    execute_on = timestep_end
  [../]
  [./stress_xz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xz
    index_i = 0
    index_j = 1
    execute_on = timestep_end
  [../]
[]

[BCs]
      [./fx]
            type = PresetBC
            variable = disp_x
            boundary = 4
            value = 0.0
      [../]

      [./fy]
            type = PresetBC
            variable = disp_y
            boundary = 1
            value = 0.0
      [../]

      [./fz]
            type = PresetBC
            variable = disp_z
            boundary = 0
            value = 0.0
      [../]

      [./pz]
            type = FunctionPresetBC
            variable = disp_z
            boundary = 5
            function = pull
      [../]
[]

[Functions]
      [./pull]
            type = PiecewiseLinear
            x = '0 1'
            y = '0 0.1'
      [../]
[]

[Materials]
  [./strain]
      type = ComputeSmallStrain
  [../]

  [./stress]
    type = TestNEMLStressSimple
  [../]
[]

[Preconditioning]
  [./pc]
    type = SMP
    full = True
  [../]
[]

[Postprocessors]
  [./stress_xx]
      type = ElementAverageValue
      variable = stress_xx
  [../]
  [./stress_yy]
      type = ElementAverageValue
      variable = stress_yy
  [../]
  [./stress_zz]
      type = ElementAverageValue
      variable = stress_zz
  [../]
  [./stress_xy]
      type = ElementAverageValue
      variable = stress_xy
  [../]
  [./stress_xz]
      type = ElementAverageValue
      variable = stress_xz
  [../]
  [./stress_yz]
      type = ElementAverageValue
      variable = stress_yz
  [../]
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
