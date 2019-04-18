#
[GlobalParams]
  order = FIRST
  family = LAGRANGE
  volumetric_locking_correction = true
  displacements = 'disp_x disp_y'
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 1
  ny = 1
  xmax = 1.0
  ymax = 1.0
  elem_type = QUAD4
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
[]

[AuxVariables]
  [./eff_plastic_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Modules/TensorMechanics/Master]
  [./all]
    strain = FINITE
    add_variables = true
    generate_output = 'stress_xx stress_yy stress_zz stress_xy vonmises_stress hydrostatic_stress elastic_strain_xx elastic_strain_yy elastic_strain_zz plastic_strain_xx plastic_strain_yy plastic_strain_zz strain_xx strain_yy strain_zz'
  [../]
[]

[AuxKernels]
  [./eff_plastic_strain]
    type = MaterialRealAux
    property = effective_plastic_strain
    variable = eff_plastic_strain
  [../]
[]

[Functions]
  [./appl_dispy]
    type = PiecewiseLinear
    x = '0     1.0     2.0     3.0'
    y = '0.0 0.208e-4 0.50e-4 1.00e-4'
  [../]
[]

[BCs]
  [./side_x]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./bot_y]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./top_y]
    type = FunctionPresetBC
    variable = disp_y
    boundary = top
    function = appl_dispy
  [../]
[]

[Materials]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    block = 0
    youngs_modulus = 250e3
    poissons_ratio = 0.25
  [../]
  [./stress]
    type = ComputeMultipleInelasticStress
    inelastic_models = 'isoplas'
    block = 0
  [../]
  [./isoplas]
    type = IsotropicPlasticityStressUpdate
    yield_stress = 5.0
    hardening_constant = 62.5e3
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu   superlu_dist'
  line_search = 'none'

  nl_rel_tol = 1e-9
  nl_abs_tol = 1e-10
  l_tol = 1e-4
  l_max_its = 100
  nl_max_its = 20

  dt = 0.1
  dtmin = 0.001
  start_time = 0.0
  end_time = 2.0

  [./Predictor]
    type = SimplePredictor
    scale = 1.0
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
  [./vonmises]
    type = ElementAverageValue
    variable = vonmises_stress
  [../]
  [./pressure]
    type = ElementAverageValue
    variable = hydrostatic_stress
  [../]
  [./el_strain_xx]
    type = ElementAverageValue
    variable = elastic_strain_xx
  [../]
  [./el_strain_yy]
    type = ElementAverageValue
    variable = elastic_strain_yy
  [../]
  [./el_strain_zz]
    type = ElementAverageValue
    variable = elastic_strain_zz
  [../]
  [./pl_strain_xx]
    type = ElementAverageValue
    variable = plastic_strain_xx
  [../]
  [./pl_strain_yy]
    type = ElementAverageValue
    variable = plastic_strain_yy
  [../]
  [./pl_strain_zz]
    type = ElementAverageValue
    variable = plastic_strain_zz
  [../]
  [./eff_plastic_strain]
    type = ElementAverageValue
    variable = eff_plastic_strain
  [../]
  [./tot_strain_xx]
    type = ElementAverageValue
    variable = strain_xx
  [../]
  [./tot_strain_yy]
    type = ElementAverageValue
    variable = strain_yy
  [../]
  [./tot_strain_zz]
    type = ElementAverageValue
    variable = strain_zz
  [../]
  [./disp_x1]
    type = NodalVariableValue
    nodeid = 0
    variable = disp_x
  [../]
  [./disp_x4]
    type = NodalVariableValue
    nodeid = 3
    variable = disp_x
  [../]
  [./disp_y1]
    type = NodalVariableValue
    nodeid = 0
    variable = disp_y
  [../]
  [./disp_y4]
    type = NodalVariableValue
    nodeid = 3
    variable = disp_y
  [../]
  [./_dt]
    type = TimestepSize
  [../]
#  [./num_lin_it]
#    type = NumLinearIterations
#  [../]
#  [./num_nonlin_it]
#    type = NumNonlinearIterations
#  [../]
#  [./tot_lin_it]
#    type = CumulativeValuePostprocessor
#    postprocessor = num_lin_it
#  [../]
#  [./tot_nonlin_it]
#    type = CumulativeValuePostprocessor
#    postprocessor = num_nonlin_it
#  [../]
#  [./alive_time]
#    type = PerfGraphData
#    section_name = Root
#    data_type = TOTAL
#  [../]
[]

[Outputs]
  exodus = true
  csv = true
  [./console]
    type = Console
    output_linear = true
  [../]
[]
