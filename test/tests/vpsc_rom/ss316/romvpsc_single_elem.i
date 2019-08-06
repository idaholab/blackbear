[Mesh]
  type = GeneratedMesh
  dim = 3
  elem_type = HEX8
  # nx = 3
  # ny = 3
  # nz = 3
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[AuxVariables]
  [./temperature]
    initial_condition = 950.0
  [../]
  [./mobile_dislocations]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./immobile_dislocations]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./effective_inelastic_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./mobile_dislocations]
    type = MaterialRealAux
    property = mobile_dislocations
    variable = mobile_dislocations
  [../]
  [./immobile_dislocations]
    type = MaterialRealAux
    property = immobile_dislocations
    variable = immobile_dislocations
  [../]
  [./effective_inelastic_strain]
    type = RankTwoScalarAux
    rank_two_tensor = creep_strain
    scalar_type = EffectiveStrain
    variable = effective_inelastic_strain
  [../]
[]

[Functions]
  [./tdisp]
    type = ParsedFunction
    # value = '1.0*t + 1.0'
    value = '1.0'
  [../]
[]

[Modules/TensorMechanics/Master]
  [./all]
    strain = FINITE
    add_variables = true
    generate_output = 'vonmises_stress'
  [../]
[]

[BCs]
  [./symmy]
    type = PresetBC
    variable = disp_y
    boundary = bottom
    value = 0
  [../]
  [./symmx]
    type = PresetBC
    variable = disp_x
    boundary = left
    value = 0
  [../]
  [./symmz]
    type = PresetBC
    variable = disp_z
    boundary = back
    value = 0
  [../]
  [./pressure_x]
    type = Pressure
    variable = disp_x
    component = 0
    boundary = right
    factor = 1.0e6
    function = tdisp
  [../]
  [./pressure_y]
    type = Pressure
    variable = disp_y
    component = 1
    boundary = top
    factor = -1.0e6
    function = tdisp
  [../]
  [./pressure_z]
    type = Pressure
    variable = disp_z
    component = 2
    boundary = front
    factor = -1.0e6
    function = tdisp
  [../]
[]

[Materials]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 3.30e11 #approximation from Clausen et al (2016) JNM 425(1-3), Table 2
    poissons_ratio = 0.3 #approximation from Clausen et al (2016) JNM 425(1-3), Table 2
  [../]
  [./stress]
    type = ComputeMultipleInelasticStress
    inelastic_models = rom_stress_prediction
  [../]
  [./rom_stress_prediction]
    type = SS316ROMStressPrediction
    temperature = temperature
    initial_mobile_dislocation_density = 6.0e12
    initial_immobile_dislocation_density = 4.4e11
    effective_inelastic_strain_name = effective_creep_strain
    internal_solve_full_iteration_history = true
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
  dt = 0.1

  solve_type = 'PJFNK'

  petsc_options_iname = -pc_hypre_type
  petsc_options_value = boomeramg
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-6
  l_tol = 1e-3
  nl_max_its = 10
  l_max_its = 10
  # end_time = 100
  dtmin = 1.0e-3
  dtmax = 1.0e-1
  timestep_tolerance = 1.0e-8
  num_steps = 2
[]

[Outputs]
  exodus = true
  csv = true
  perf_graph = true
[]

[Postprocessors]
  [./mobile_dislocations]
    type = ElementAverageValue
    variable = mobile_dislocations
  [../]
  [./immobile_dislocations]
    type = ElementAverageValue
    variable = immobile_dislocations
  [../]
  [./vonmises_stress]
    type = ElementAverageValue
    variable = vonmises_stress
  [../]
  [./effective_inelastic_strain]
    type = ElementAverageValue
    variable = effective_inelastic_strain
  [../]
[]
