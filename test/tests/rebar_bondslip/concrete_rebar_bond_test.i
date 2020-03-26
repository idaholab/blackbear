[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  file = gold/geo.e
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
[]

# [Functions]
#   [E]
#     type = PiecewiseMultilinear
#     data_file = 'gold/E.txt'
#   []
# []

[AuxVariables]
  [stress_xx]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_yy]
    order = CONSTANT
    family = MONOMIAL
  []
  [strain_xx]
    order = CONSTANT
    family = MONOMIAL
  []
  [strain_yy]
    order = CONSTANT
    family = MONOMIAL
  []
  [area]
    order = CONSTANT
    family = MONOMIAL
  []
  [damage_index]
    order = CONSTANT
    family = MONOMIAL
  []
  [./axial_stress]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [stress_xx]
    type = RankTwoAux
    index_i = 0
    index_j = 0
    variable = stress_xx
    execute_on = 'TIMESTEP_END'
    rank_two_tensor = stress
    block = 1
  []
  [stress_yy]
    type = RankTwoAux
    index_i = 1
    index_j = 1
    variable = stress_yy
    execute_on = 'TIMESTEP_END'
    rank_two_tensor = stress
    block = 1
  []
  [strain_xx]
    type = RankTwoAux
    index_i = 0
    index_j = 0
    variable = strain_xx
    execute_on = 'TIMESTEP_END'
    rank_two_tensor = total_strain
    block = 1
  []
  [strain_yy]
    type = RankTwoAux
    index_i = 1
    index_j = 1
    variable = strain_yy
    execute_on = 'TIMESTEP_END'
    rank_two_tensor = total_strain
    block = 1
  []
  [area]
    type = ConstantAux
    block = 2
    variable = area
    value = 7.07e-4
    execute_on = 'initial'
  []
  [damage_index]
    type = MaterialRealAux
    variable = damage_index
    property = damage_index
    block = 1
    execute_on = timestep_end
  []
  [./axial_stress]
    type = MaterialRealAux
    block = '2'
    variable = axial_stress
    property = axial_stress
  [../]
[]

[Materials]
  [concrete_strain]
    type = ComputeIncrementalSmallStrain
    block = 1
  []
  [Cijkl_concrete]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 500e6
    poissons_ratio = 0.2
    block = 1
  []
  [damage]
    type = MazarsDamage
    use_old_damage = true
    tensile_strength = 1e6
    a_t = 0.87
    a_c = 0.65
    b_t = 20000
    b_c = 2150
    block = 1
  []
  [concrete_stress]
    type = ComputeDamageStress
    damage_model = damage
    block = 1
  []
  # [concrete_stress]
  #   type = ComputeFiniteStrainElasticStress
  #   block = 1
  # []
  # [truss]
  #   type = LinearElasticTruss
  #   block = 2
  #   youngs_modulus = 2e11
  # []
  [./truss]
    type = PlasticTruss
    youngs_modulus = 2.0e11
    yield_stress = 500e5
    hardening_constant = 100
    block = '2'
    outputs = exodus
  [../]
[]

[Postprocessors]
  [./stress_xx]
    type = ElementAverageValue
    variable = stress_xx
    block = '1'
  [../]
  [./strain_xx]
    type = ElementAverageValue
    variable = strain_xx
    block = '1'
  [../]
  [./stress_yy]
    type = ElementAverageValue
    variable = stress_yy
    block = '1'
  [../]
  [./strain_yy]
    type = ElementAverageValue
    variable = strain_yy
    block = '1'
  [../]
  [./axial_stress]
    type = ElementAverageValue
    variable = axial_stress
    block = '2'
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  line_search = basic
  # petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart -mat_mffd_type'
  # petsc_options_value = 'hypre    boomeramg      101                ds'

  # petsc_options_iname = '-pc_type -ksp_gmres_restart'
  # petsc_options_value = 'jacobi       101'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  nl_rel_tol = 1e-5
  nl_abs_tol = 1e-6
  l_tol = 1e-03
  l_max_its = 100
  nl_max_its = 30
  dt = 2e-5
  end_time = 1
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
  file_base = concrete_rebar_bond_test
[]

[Kernels]
  [solid_x_concrete]
    type = StressDivergenceTensors
    block = 1
    component = 0
    variable = disp_x
    use_displaced_mesh = true
  []
  [solid_y_concrete]
    type = StressDivergenceTensors
    block = 1
    component = 1
    variable = disp_y
    use_displaced_mesh = true
  []
  [solid_x_truss]
    type = StressDivergenceTensorsTruss
    block = 2
    component = 0
    variable = disp_x
    area = area
  []
  [solid_y_truss]
    type = StressDivergenceTensorsTruss
    block = 2
    component = 1
    variable = disp_y
    area = area
  []
[]

[BCs]
  [concrete_top_ydisp]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = '1'
    function = '-t'
  []
  [concrete_bottom_yfix]
    type = DirichletBC
    variable = disp_y
    boundary = '2 3'
    value = 0
  []
  [concrete_bottom_xfix]
    type = DirichletBC
    variable = disp_x
    boundary = '2'
    value = 0
  []
[]

[Constraints]
  [rebar_x]
    type = RebarBondSlipConstraint
    slave = 2
    master = 1
    penalty = 1e6
    variable = 'disp_x'
    master_variable = 'disp_x'
    component = 0
    max_bondstress = 3.5e-10
    transitional_slip_values = 0.05e-3
    ultimate_slip = 1.0
    rebar_radius = 0.015
    frictional_bondstress = 1e-15
  []
  [rebar_y]
    type = RebarBondSlipConstraint
    slave = 2
    master = 1
    penalty = 1e6
    variable = 'disp_y'
    master_variable = 'disp_y'
    component = 1
    max_bondstress = 3.5e-10
    transitional_slip_values = 0.05e-3
    ultimate_slip = 1.0
    rebar_radius = 0.015
    frictional_bondstress = 1e-15
  []
  # [rebar_x]
  #   type = EqualValueEmbeddedConstraint
  #   slave = 2
  #   master = 1
  #   penalty = 1e6
  #   variable = 'disp_x'
  #   master_variable = 'disp_x'
  #   formulation = penalty
  # []
  # [rebar_y]
  #   type = EqualValueEmbeddedConstraint
  #   slave = 2
  #   master = 1
  #   penalty = 1e6
  #   variable = 'disp_y'
  #   master_variable = 'disp_y'
  #   formulation = penalty
  # []
[]
