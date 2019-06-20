[Mesh]
  type = GeneratedMesh
  dim = 3
  elem_type = HEX8
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[AuxVariables]
  [./temperature]
    initial_condition = 800.0
  [../]
[]

[Functions]
  [./tdisp]
    type = ParsedFunction
    value = '100.0*t + 100.0'
    # value = '0.01*t'
  [../]
[]

[Modules/TensorMechanics/Master]
  [./all]
    strain = FINITE
    add_variables = true
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
    factor = 6.336e5
    function = tdisp
  [../]
  [./pressure_z]
    type = Pressure
    variable = disp_z
    component = 2
    boundary = front
    factor = 6.336e5
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
    youngs_modulus = 3.30e11 #approximation from Clausen et al (2016) JNM 425(1-3), Table 2
    poissons_ratio = 0.3 #approximation from Clausen et al (2016) JNM 425(1-3), Table 2
  [../]
  [./rom_stress_prediction]
    type = SS316ThinWallROMStressPrediction
    temperature = temperature
    initial_mobile_dislocation_density = 2.58e12
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
  dt = 0.05

  solve_type = 'PJFNK'

  petsc_options_iname = -pc_hypre_type
  petsc_options_value = boomeramg
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-6
  l_tol = 1e-3
  dtmax = 10.0
  end_time = 100
  dtmin = 1.0e-2
  num_steps = 10
[]

[Outputs]
  exodus = true
  csv = true
[]
