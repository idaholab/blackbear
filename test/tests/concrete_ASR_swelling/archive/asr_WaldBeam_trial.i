[Mesh]
  type = FileMesh
  file = mesh_WaldBeam.e
  displacements = 'disp_x disp_y disp_z'
  # block 1: concrete
  # block 2: Long. steel reinforcement
  # block 3: Trans. steel reinforcement
  # block 4: Headed trans. steel reinforcement
  # nodeset 1: Ends of headed reinforcement
  # nodeset 2: BC @(front, left,   bottom) for (x, y, and z)
  # nodeset 3: BC @(back, right, bottom) for (y and z)
  # nodeset 4: BC @(front, right, bottom) for (z)
  # nodeset 5: BC @top surface
  # type = GeneratedMesh
  #dim = 3
  #nx = 1
  #ny = 1
  #nz = 1
  #xmin = -0.5
  #xmax = 0.5
  #ymin = -0.5
  #ymax = 0.5
  #zmin = -0.5
  #zmax = 0.5
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
[]

[Kernels]
  [./TensorMechanics]
    displacements = 'disp_x disp_y disp_z'
  [../]
[]

[AuxVariables]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./yield_fcn]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./e_over_l]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./area]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./react_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./react_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./react_z]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxKernels]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    block = '1'
    index_i = 0
    index_j = 0
  [../]
  [./stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
    block = '1'
    index_i = 0
    index_j = 1
  [../]
  [./stress_xz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xz
    block = '1'
    index_i = 0
    index_j = 2
  [../]
  [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    block = '1'
    index_i = 1
    index_j = 1
  [../]
  [./stress_yz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yz
    block = '1'
    index_i = 1
    index_j = 2
  [../]
  [./stress_zz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zz
    block = '1'
    index_i = 2
    index_j = 2
  [../]
  [./yield_fcn_auxk]
    type = MaterialStdVectorAux
    index = 0
    property = plastic_yield_function
    variable = yield_fcn
    block = '1'
  [../]
  [./axial_stress]
    type = MaterialRealAux
    block = '2 3 4'
    property = axial_stress
    variable = axial_stress
  [../]
  [./e_over_l]
    type = MaterialRealAux
    block = '2 3 4'
    property = e_over_l
    variable = e_over_l
  [../]
  [./area]
    type = ConstantAux
    block = '1 2'
    variable = area
    value = 1.0
    execute_on = 'initial timestep_begin'
  [../]
[]

[Postprocessors]
  [./s_xx]
    type = PointValue
    point = '0 0 0'
    variable = stress_xx
  [../]
  [./s_xy]
    type = PointValue
    point = '0 0 0'
    variable = stress_xy
  [../]
  [./s_xz]
    type = PointValue
    point = '0 0 0'
    variable = stress_xz
  [../]
  [./s_yy]
    type = PointValue
    point = '0 0 0'
    variable = stress_yy
  [../]
  [./s_yz]
    type = PointValue
    point = '0 0 0'
    variable = stress_yz
  [../]
  [./s_zz]
    type = PointValue
    point = '0 0 0'
    variable = stress_zz
  [../]
  [./f]
    type = PointValue
    point = '0 0 0'
    variable = yield_fcn
  [../]
  [./f0]
    type = PointValue
    point = '0 0 0'
    variable = yield_fcn
  [../]
  [./f1]
    type = PointValue
    point = '0 0 0'
    variable = yield_fcn
  [../]
[]

[BCs]
  [./xyz]
    type = FunctionDirichletBC
    variable = 'disp_x disp_y disp_z'
    boundary = '2'
    function = '0'
  [../]
  [./yz]
    type = FunctionDirichletBC
    variable = 'disp_y disp_z'
    boundary = '3'
    function = '0'
  [../]
  [./z]
    type = FunctionDirichletBC
    variable = disp_z
    boundary = '4'
    function = '0'
  [../]
  [./pressure]
    type = FunctionDirichletBC
    variable = disp_z
    boundary = '5'
    function = '-1*t'
  [../]
[]

[UserObjects]
  [./mc_coh]
    type = TensorMechanicsHardeningConstant
    value = 10
  [../]
  [./mc_phi]
    type = TensorMechanicsHardeningConstant
    value = 20
    convert_to_radians = true
  [../]
  [./mc_psi]
    type = TensorMechanicsHardeningConstant
    value = 0
  [../]
  [./mc]
    type = TensorMechanicsPlasticDruckerPragerHyperbolic
    mc_cohesion = mc_coh
    mc_friction_angle = mc_phi
    mc_dilation_angle = mc_psi
    smoother = 4
    mc_interpolation_scheme = inner_edge
    yield_function_tolerance = 1E-5
    internal_constraint_tolerance = 1E-11
  [../]
[]

[Materials]
  [./elasticity_tensor]
    type = ComputeElasticityTensor
    block = 1
    fill_method = symmetric_isotropic
    C_ijkl = '0 1E7'
  [../]
  [./strain]
    type = ComputeIncrementalSmallStrain
    block = 1
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./mc]
    type = ComputeMultiPlasticityStress
    block = 1
    ep_plastic_tolerance = 1E-12
    plastic_models = mc
    debug_fspb = crash
  [../]
  [./linelast]
    type = LinearElasticTruss
    block = '2 3 4'
    youngs_modulus = 1e6
    displacements = 'disp_x disp_y disp_z'
  [../]
[]

[Modules/TensorMechanics/LineElementMaster]
  [./block]
    truss = true
    add_variables = true
    displacements = 'disp_x disp_y disp_z'

    area = area

    block = '2 3 4'

    save_in = 'react_x react_y react_z'
  [../]
[]

[Executioner]
  end_time = 100
  dt = 1
  type = Transient
[]


[Outputs]
  file_base = out_asr_WaldBeam_trial
  exodus = true
  [./csv]
    type = CSV
  [../]
[]
