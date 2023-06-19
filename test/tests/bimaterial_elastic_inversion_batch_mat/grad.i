[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 11
    ny = 11
    xmin = -4
    xmax = 4
    ymin = -4
    ymax = 4
  []
[]

[Variables]
  [ux]
  []
  [uy]
  []
[]

[AuxVariables]
  [state_x]
  []
  [state_y]
  []
  [dummy]
  []
[]

[DiracKernels]
  [misfit_is_adjoint_force]
    type = ReporterPointSource
    variable = ux
    x_coord_name = misfit/measurement_xcoord
    y_coord_name = misfit/measurement_ycoord
    z_coord_name = misfit/measurement_zcoord
    value_name = misfit/misfit_values
  []
[]

[Kernels]
  [div_sigma_x]
    type = StressDivergenceTensors
    variable = ux
    displacements = 'ux uy'
    component = 0
    volumetric_locking_correction = false
  []
  [div_sigma_y]
    type = StressDivergenceTensors
    variable = uy
    displacements = 'ux uy'
    component = 1
    volumetric_locking_correction = false
  []
[]

[BCs]
  [bottom_ux]
    type = DirichletBC
    variable = ux
    boundary = bottom
    value = 0.0
  []
  [bottom_uy]
    type = DirichletBC
    variable = uy
    boundary = bottom
    value = 0.0
  []
[]

[Materials]
  [stress]
    type = ComputeLinearElasticStress
  []
  [strain]
    type = ComputeSmallStrain
    displacements = 'ux uy'
  []
  [elasticity_tensor]
    type = ComputeVariableIsotropicElasticityTensor
    args = dummy
    youngs_modulus = E_material
    poissons_ratio = nu_material
  []
  [E_material]
    type = GenericFunctionMaterial
    prop_names = 'E_material'
    prop_values = E
  []
  [nu_material]
    type = GenericFunctionMaterial
    prop_names = 'nu_material'
    prop_values = nu
  []
  [forward_strain]
    type = ComputeSmallStrain
    displacements = 'state_x state_y'
    base_name = 'forward'
  []
  # Below are the gradients of elasticity tensor for this elastic inversion problem
  [dC_dlambda]
    type = ComputeElasticityTensor
    C_ijkl = '1 1 1 1 1 1 0 0 0'
    fill_method = symmetric9
    base_name = 'dC_dlambda'
  []
  [dC_dmu]
    type = ComputeElasticityTensor
    C_ijkl = '2 0 0 2 0 2 1 1 1'
    fill_method = symmetric9
    base_name = 'dC_dmu'
  []
[]

[Functions]
  [E]
    type = ParsedFunction
    expression = mu*(3*lambda+2*mu)/(lambda+mu)
    symbol_names = 'lambda mu'
    symbol_values = 'lambda mu'
  []
  [nu]
    type = ParsedFunction
    expression = lambda/2/(lambda+mu)
    symbol_names = 'lambda mu'
    symbol_values = 'lambda mu'
  []
  [lambda]
    type = NearestReporterCoordinatesFunction
    x_coord_name = parametrization/coordx
    y_coord_name = parametrization/coordy
    value_name = parametrization/lambda
  []
  [mu]
    type = NearestReporterCoordinatesFunction
    x_coord_name = parametrization/coordx
    y_coord_name = parametrization/coordy
    value_name = parametrization/mu
  []
[]

[Reporters]
  [measure_data]
    type = OptimizationData
    variable = ux
  []
  [misfit]
    type = OptimizationData
  []
  [parametrization]
    type = ConstantReporter
    real_vector_names = 'coordx coordy lambda mu'
    real_vector_values = '0 1 2; 0 1 2; 5 4 3; 1 2 3'
  []
[]

# Below is the part where a customized userObject that takes NEML stress gradient
# and transforms it into a batch material that has the output type as RankTwoTensor
# One userObject per gradient material property from NEML
[UserObjects]
  [stress_grad_lambda]
    type = BatchStressGrad
    elasticity_tensor_derivative = 'dC_dlambda_elasticity_tensor' # calculated in dC_dlambda
    execution_order_group = -1 # need to make sure that the UO executes before the VPP
  []
  [stress_grad_mu]
    type = BatchStressGrad
    elasticity_tensor_derivative = 'dC_dmu_elasticity_tensor' # calculated in dC_dmu
    execution_order_group = -1 # need to make sure that the UO executes before the VPP
  []
[]

# Below is where the stress gradient batch material objects are utilized in the
# actual gradient calculations
[VectorPostprocessors]
  [grad_lambda]
    type = AdjointStrainStressGradInnerProduct
    stress_grad_name = 'stress_grad_lambda'
    adjoint_strain_name = 'mechanical_strain'
    variable = dummy
    function = lambda
  []
  [grad_mu]
    type = AdjointStrainStressGradInnerProduct
    stress_grad_name = 'stress_grad_mu'
    adjoint_strain_name = 'mechanical_strain'
    variable = dummy
    function = mu
  []
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
[]

[Outputs]
  file_base = 'adjoint'
  console = false
[]
