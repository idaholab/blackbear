[Mesh]
  file = ./gold/mesh_RCBeam.e
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  volumetric_locking_correction = true
[]

[Modules/TensorMechanics/Master]
  [./Concrete_block]
    block = 1
    strain = small
    incremental = true
    generate_output = 'stress_xx stress_xy stress_xz stress_yy stress_yz stress_zz
               strain_xx strain_xy strain_xz strain_yy strain_yz strain_zz
               max_principal_stress mid_principal_stress min_principal_stress
               secondinv_stress thirdinv_stress vonmises_stress
               secondinv_strain thirdinv_strain
               elastic_strain_xx elastic_strain_xy elastic_strain_xz elastic_strain_yy elastic_strain_yz elastic_strain_zz'
    save_in = 'resid_x resid_y resid_z'
  [../]
[]

[Modules/TensorMechanics/LineElementMaster]
  [./Reinforcement_block]
    block = '2 3 4 5'
    truss = true
    area = area
    displacements = 'disp_x disp_y disp_z'
    save_in = 'resid_x resid_y resid_z'
  [../]
[]

[Constraints/EqualValueEmbeddedConstraint/EqualValueEmbeddedConstraintAction]
  primary_block = '1'
  secondary_block = '2 3 4 5'
  primary_variable = 'disp_x disp_y disp_z'
  displacements = 'disp_x disp_y disp_z'
  penalty = 1e12
  formulation = penalty
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
[]

[AuxVariables]
  [./resid_x]
  [../]
  [./resid_y]
  [../]
  [./resid_z]
  [../]
  [./area]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./D]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./intnl0]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./intnl1]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./area2]
    type = ConstantAux
    block = 2
    variable = area
    value = 5.09e-4 # 509 mm2
    execute_on = 'initial timestep_begin'
  [../]
  [./area3]
    type = ConstantAux
    block = 3
    variable = area
    value = 2.00e-4 # 200 mm2
    execute_on = 'initial timestep_begin'
  [../]
  [./area4]
    type = ConstantAux
    block = 4
    variable = area
    value = 0.71e-4 # 71 mm2
    execute_on = 'initial timestep_begin'
  [../]
  [./area5]
    type = ConstantAux
    block = 5
    variable = area
    value = 2.00e-4 # 200 mm2
    execute_on = 'initial timestep_begin'
  [../]
[]

[BCs]
  [./loading]
    type = FunctionPresetBC
    variable = disp_y
    boundary = '1'
    function = '-1E-5*t'
  [../]
  [./left_support_x]
    type = FunctionPresetBC
    variable = disp_x
    boundary = '3'
    function = '0.'
  [../]
  [./left_support_y]
    type = FunctionPresetBC
    variable = disp_y
    boundary = '3'
    function = '0.'
  [../]
  [./left_support_z]
    type = FunctionPresetBC
    variable = disp_z
    boundary = '3'
    function = '0.'
  [../]
  [./right_support_y]
    type = FunctionPresetBC
    variable = disp_y
    boundary = '2'
    function = '0.'
  [../]
  [./right_support_z]
    type = FunctionPresetBC
    variable = disp_z
    boundary = '2'
    function = '0.'
  [../]
[]

[Postprocessors]
  [./deformation_x]
    type = AverageNodalVariableValue
    variable = disp_y
    boundary = '1'
  [../]
  [./react_x]
    type = NodalSum
    variable = resid_y
    boundary = '2 3'
  [../]
[]

[Materials]
  # Material Properties of concrete
  [./elasticity_tensor_concrete]
    type = ComputeIsotropicElasticityTensor
    block = 1
    youngs_modulus = 3.1E10
    poissons_ratio = 0.18
  [../]
  [./stress_concrete]
    type = ComputeFiniteStrainElasticStress
    block = 1
  [../]

  # Material Properties of steel reinforcement
  [./truss]
    type = LinearElasticTruss
    block = '2 3 4 5'
    youngs_modulus = 2e11
  [../]
[]

[Preconditioning]
  active = SMP
  [./SMP]
    type = SMP
    full = true
  [../]
  [./FDP]
    type = FDP
    full = true
  [../]
[]

[Executioner]
  solve_type = 'NEWTON'
  nl_max_its = 100
  nl_abs_tol = 1.E-5
  nl_rel_tol = 1E-3

  line_search = none

  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  petsc_options = '-snes_converged_reason'

  type = Transient
  end_time = 200
  dt = 1
[]

[Outputs]
  print_linear_residuals = false
  exodus = true
  [./csv]
    type = CSV
  [../]
[]
