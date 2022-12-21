[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 1
  xmin = -0.5
  xmax = 0.5
  ymin = -0.5
  ymax = 0.5
  zmin = -0.5
  zmax = 0.5
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Modules/TensorMechanics/Master]
  [all]
    add_variables = true
    incremental = true
    generate_output = 'stress_xx stress_xy stress_xz stress_yy stress_yz stress_zz plastic_strain_xx plastic_strain_xy plastic_strain_xz plastic_strain_yy plastic_strain_yz plastic_strain_zz strain_xx strain_xy strain_xz strain_yy strain_yz strain_zz volumetric_strain'
    save_in = 'resid_x resid_y resid_z'
  []
[]

[AuxVariables]
  [resid_x]
  []
  [resid_y]
  []
  [resid_z]
  []
  [min_ep]
    order = CONSTANT
    family = MONOMIAL
  []
  [mid_ep]
    order = CONSTANT
    family = MONOMIAL
  []
  [max_ep]
    order = CONSTANT
    family = MONOMIAL
  []
  [sigma0]
    order = CONSTANT
    family = MONOMIAL
  []
  [sigma1]
    order = CONSTANT
    family = MONOMIAL
  []
  [sigma2]
    order = CONSTANT
    family = MONOMIAL
  []
  [f0]
    order = CONSTANT
    family = MONOMIAL
  []
  [D]
    order = CONSTANT
    family = MONOMIAL
  []
  [intnl0]
    order = CONSTANT
    family = MONOMIAL
  []
  [intnl1]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [f0_auxk]
    type = MaterialStdVectorAux
    property = plastic_yield_function
    index = 0
    variable = f0
  []
  [D_auxk]
    type = MaterialRealAux
    property = damage_variable
    variable = D
  []
  [min_ep]
    type = MaterialRealAux
    property = min_ep
    variable = min_ep
  []
  [mid_ep]
    type = MaterialRealAux
    property = mid_ep
    variable = mid_ep
  []
  [max_ep]
    type = MaterialRealAux
    property = max_ep
    variable = max_ep
  []
  [sigma0]
    type = MaterialRealAux
    property = damaged_min_principal_stress
    variable = sigma0
  []
  [sigma1]
    type = MaterialRealAux
    property = damaged_mid_principal_stress
    variable = sigma1
  []
  [sigma2]
    type = MaterialRealAux
    property = damaged_max_principal_stress
    variable = sigma2
  []
  [intnl0_auxk]
    type = MaterialRealAux
    property = damage_state_in_tension
    variable = intnl0
  []
  [intnl1_auxk]
    type = MaterialRealAux
    property = damage_state_in_compression
    variable = intnl1
  []
[]

[Postprocessors]
  [react_x]
    type = NodalSum
    variable = resid_x
    boundary = 'left'
  []
  [displacement_x]
    type = AverageNodalVariableValue
    variable = disp_x
    boundary = 'right'
  []
  [s_xx]
    type = PointValue
    point = '0 0 0'
    variable = stress_xx
  []
  [e_xx]
    type = PointValue
    point = '0 0 0'
    variable = strain_xx
  []
  [ep_xx]
    type = PointValue
    point = '0 0 0'
    variable = plastic_strain_xx
  []
  [volumetric_strain]
    type = PointValue
    point = '0 0 0'
    variable = volumetric_strain
  []
[]

[BCs]
  [x_r]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = 'right'
    function = '1E-4*x*t'
  []
  [x_l]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = 'left'
    function = '0'
  []
  [y_l]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = 'bottom'
    function = '0'
  []
  [z_l]
    type = FunctionDirichletBC
    variable = disp_z
    boundary = 'back'
    function = '0'
  []
[]

[Materials]
  [elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 3.17E4
    poissons_ratio = 0.18
  []
  [damage_plasticity_model]
    type = DamagePlasticityStressUpdate
    biaxial_uniaxial_compressive_stress_factor = 0.109
    dilatancy_factor = 0.23
    stiff_recovery_factor = 0.001

    yield_strength_in_tension = 3.48
    ft_ep_slope_factor_at_zero_ep = 0.70
    tensile_damage_at_half_tensile_strength = 0.51
    fracture_energy_in_tension = 12.3E-3

    yield_strength_in_compression = 18.30
    maximum_strength_in_compression = 27.60
    compressive_damage_at_max_compressive_strength = 0.25
    fracture_energy_in_compression = 6.889E-2

    yield_function_tol = 1.E-5
    tip_smoother = 1.E-6
    smoothing_tol = 1.E-3
  []
  [stress]
    type = ComputeMultipleInelasticDamageStress
    inelastic_models = damage_plasticity_model
    perform_finite_strain_rotations = false
  []
[]

[Executioner]
  type = Transient
  end_time = 100
  dt = 2
[]

[Outputs]
  csv = true
[]
