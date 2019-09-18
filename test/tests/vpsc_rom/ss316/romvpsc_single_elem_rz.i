[Mesh]
  type = GeneratedMesh
  dim = 2
  elem_type = Quad4
[]

[Problem]
  coord_type = RZ
[]

[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[AuxVariables]
  [./temperature]
    initial_condition = 800.0
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
    value = '10.0*t + 50.0'
    # value = '0.01*t'
  [../]
[]

[Modules/TensorMechanics/Master]
  [./all]
    strain = FINITE
    add_variables = true
    generate_output = 'vonmises_stress strain_xx strain_xy strain_yy strain_zz'
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
  [./pressure_x]
    type = Pressure
    variable = disp_x
    component = 0
    boundary = right
    factor = 3.1675e5
    function = tdisp
  [../]
  [./pressure_y]
    type = Pressure
    variable = disp_y
    component = 1
    boundary = top
    factor = 6.336e5
    function = tdisp
  [../]
[]

[Materials]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1.80e11 #from Takahashi et al (2006) Fusion Engineering and Design 81(1-7), Table 2
    poissons_ratio = 0.3 #from Takahashi et al (2006) Fusion Engineering and Design 81(1-7), Table 2
  [../]
  [./stress]
    type = ComputeMultipleInelasticStress
    inelastic_models = rom_stress_prediction
  [../]
  [./rom_stress_prediction]
    type = SS316ROMStressPrediction
    temperature = temperature
    initial_mobile_dislocation_density = 7.58e12
    initial_immobile_dislocation_density = 6.98e11
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

  petsc_options_iname = '-pc_type -pc_asm_overlap -sub_pc_type -ksp_type -ksp_gmres_restart'
  petsc_options_value = ' asm      1              lu            gmres     200'

  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-6
  l_tol = 1e-3
  dt = 0.05
  end_time = 100
  dtmax = 10.0
  dtmin = 1.0e-2
  # num_steps = 10
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
  [./strain_xx]
    type = ElementAverageValue
    variable = strain_xx
  [../]
  [./strain_yy]
    type = ElementAverageValue
    variable = strain_yy
  [../]
  [./strain_zz]
    type = ElementAverageValue
    variable = strain_zz
  [../]
[]
