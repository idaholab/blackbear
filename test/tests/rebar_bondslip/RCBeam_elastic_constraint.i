[Mesh]
  file = gold/RCBeam_test_mesh.e
[]

[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Modules]
  [TensorMechanics]
    [Master]
      [Concrete_block]
        block = 1
        strain = small
        incremental = true
        generate_output = 'stress_xx strain_xx'
      []
    []
  []
[]

[Physics]
  [SolidMechanics]
    [LineElement]
      [QuasiStatic]
        [Reinforcement_block]
          block = '2'
          truss = true
          area = area
          displacements = 'disp_x disp_y'
        []
      []
    []
  []
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
[]

[AuxVariables]
  [area]
    order = CONSTANT
    family = MONOMIAL
  []
  [axial_stress]
    order = CONSTANT
    family = MONOMIAL
  []
  [axial_total_stretch]
    order = CONSTANT
    family = MONOMIAL
  []
  [axial_elastic_stretch]
    order = CONSTANT
    family = MONOMIAL
  []
  [output_axial_slipx]
  []
  [output_axial_forcex]
  []
  [output_axial_plastic_slipx]
  []
  [output_axial_slipy]
  []
  [output_axial_forcey]
  []
[]

[AuxKernels]
  [area]
    type = ConstantAux
    block = '2'
    variable = area
    value = 2.00e-4
    execute_on = 'initial timestep_begin'
  []
  [axial_stress]
    type = MaterialRealAux
    block = '2'
    variable = axial_stress
    property = axial_stress
  []

  [axial_total_stretch]
    type = MaterialRealAux
    block = '2'
    property = total_stretch
    variable = axial_total_stretch
  []
  [axial_elastic_stretch]
    type = MaterialRealAux
    block = '2'
    property = elastic_stretch
    variable = axial_elastic_stretch
  []
[]

[Constraints]
  [rebar_x]
    type = RebarBondSlipConstraint
    secondary = 2
    primary = 1
    penalty = 1e6
    variable = 'disp_x'
    primary_variable = 'disp_x'
    max_bondstress = 1e6
    transitional_slip_value = 5e-5
    rebar_radius = 2.00e-4
    formulation = PENALTY
    bondslip_model = concrete_rebar_model
    output_axial_slip = output_axial_slipx
    output_axial_force = output_axial_forcex
    output_axial_plastic_slip = output_axial_plastic_slipx
  []
  [rebar_y]
    type = RebarBondSlipConstraint
    secondary = 2
    primary = 1
    penalty = 1e6
    variable = 'disp_y'
    primary_variable = 'disp_y'
    max_bondstress = 1e3
    transitional_slip_value = 5e-5
    rebar_radius = 2.00e-4
    formulation = PENALTY
    bondslip_model = concrete_rebar_model
  []
[]

[Functions]
  [loading]
    type = PiecewiseLinear
    x = '0 10       20     30 '
    y = '0 0.00008 -0.0001 0.0'
  []
[]

[BCs]
  [loading]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = '102'
    function = loading
    preset = true
  []
  [left_support_x]
    type = DirichletBC
    variable = disp_x
    boundary = '100'
    value = 0
  []
  [left_support_y]
    type = DirichletBC
    variable = disp_y
    boundary = '100'
    value = 0
  []
[]

[Materials]
  [Cijkl_concrete]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 500e6
    poissons_ratio = 0.2
    block = '1'
  []
  [stress]
    type = ComputeFiniteStrainElasticStress
    block = '1'
  []
  [truss]
    type = LinearElasticTruss
    block = '2'
    youngs_modulus = 2e11
  []
[]

[Postprocessors]
  [stress_xx]
    type = ElementAverageValue
    variable = stress_xx
    block = '1'
  []
  [strain_xx]
    type = ElementAverageValue
    variable = strain_xx
    block = '1'
  []
  [axial_stress]
    type = ElementAverageValue
    variable = axial_stress
    block = '2'
  []
  [axial_elastic_stretch]
    type = ElementAverageValue
    variable = axial_elastic_stretch
    block = '2'
  []
  [axial_total_stretch]
    type = ElementAverageValue
    variable = axial_total_stretch
    block = '2'
  []
  [node_slipx]
    type = NodalVariableValue
    variable = output_axial_slipx
    nodeid = 152
  []
  [node_forcex]
    type = NodalVariableValue
    variable = output_axial_forcex
    nodeid = 152
  []
  [node_plastic_slipx]
    type = NodalVariableValue
    variable = output_axial_plastic_slipx
    nodeid = 152
  []
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  nl_max_its = 20
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-05
  l_tol = 1e-03
  line_search = none
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  petsc_options = '-snes_converged_reason'
  end_time = 30
  dt = 0.1
  num_steps = 1
[]

[Outputs]
  csv = true
[]
