[Mesh]
  [meshfile]
    type = FileMeshGenerator
    file = gold/RCBeam_test_mesh.e
  []
  [rotate]
    type = TransformGenerator
    input = meshfile
    transform = 'rotate'
    vector_value = '45 0 0'
  []
[]

[GlobalParams]
  displacements = 'disp_x disp_y'
  # volumetric_locking_correction = true
[]

[Modules]

  [TensorMechanics]

    [Master]
      [Concrete_block]
        block = 1
        # strain = small
        strain = finite
        incremental = true
        # add_variables = true
        generate_output = 'stress_xx stress_xy stress_yy strain_xx strain_xy strain_yy
                     max_principal_stress mid_principal_stress min_principal_stress
                     secondinv_stress thirdinv_stress vonmises_stress
                     secondinv_strain thirdinv_strain
                     elastic_strain_xx elastic_strain_xy elastic_strain_yy'
        #    		       plastic_strain_xx plastic_strain_xy plas tic_strain_xz plastic_strain_yy plastic_strain_yz plastic_strain_zz'
        save_in = 'resid_x resid_y'
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
          save_in = 'resid_x resid_y'
          # add_variables = true
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
  [resid_x]
  []
  [resid_y]
  []
  [area]
    order = CONSTANT
    family = MONOMIAL
  []
  [axial_stress_1]
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
[]

[AuxKernels]
  [area]
    type = ConstantAux
    block = '2'
    variable = area
    value = 2.00e-4
    execute_on = 'initial timestep_begin'
  []
  [axial_stress_1]
    type = MaterialRealAux
    block = '2'
    variable = axial_stress_1
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
    component = 0
    max_bondstress = 1e4
    transitional_slip_values = 0.00005
    ultimate_slip = 0.5
    rebar_radius = 7.98e-3
    frictional_bondstress = 100
  []
  [rebar_y]
    type = RebarBondSlipConstraint
    secondary = 2
    primary = 1
    penalty = 1e6
    variable = 'disp_y'
    primary_variable = 'disp_y'
    component = 1
    max_bondstress = 1e4
    transitional_slip_values = 0.00005
    ultimate_slip = 0.5
    rebar_radius = 7.98e-3
    frictional_bondstress = 100
  []
[]

[Functions]
  [loading]
    type = PiecewiseLinear
    x = '0 10       20     30 '
    y = '0 ${fparse 0.0001414213562373095/2} ${fparse -0.0001414213562373095/2} 0.0'
  []
[]

[BCs]
  [loadingx]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = '102'
    function = loading
    preset = true
  []
  [loadingy]
    type = FunctionDirichletBC
    variable = disp_y
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

[Postprocessors]
  [deformation_x]
    type = AverageNodalVariableValue
    variable = disp_x
    boundary = '101'
  []
  [deformation_y]
    type = AverageNodalVariableValue
    variable = disp_y
    boundary = '101'
  []
  [react_x]
    type = AverageNodalVariableValue
    variable = resid_x
    boundary = '100'
  []
  [react_y]
    type = AverageNodalVariableValue
    variable = resid_y
    boundary = '100'
  []
  [react_x2]
    type = AverageNodalVariableValue
    variable = resid_x
    boundary = '101'
  []
  [react_y2]
    type = AverageNodalVariableValue
    variable = resid_y
    boundary = '100'
  []
  [node1_fx]
    type = NodalVariableValue
    variable = resid_x
    # boundary = '1001'
    nodeid = 138
  []
  [node1_fy]
    type = NodalVariableValue
    variable = resid_y
    nodeid = 138
  []
  [node1_dx]
    type = NodalVariableValue
    variable = disp_x
    nodeid = 138
  []
  [node1_dy]
    type = NodalVariableValue
    variable = disp_y
    nodeid = 138
  []
  [node1_fx2]
    type = AverageNodalVariableValue
    variable = resid_x
    boundary = '102'
  []
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
    variable = axial_stress_1
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
  dtmin = 0.00001
  # num_steps=1
  dt = 0.1
[]

[Outputs]
  # print_linear_residuals = false
  exodus = true
  csv = true
  # file_base = RCBeam_bondslip_test
[]
