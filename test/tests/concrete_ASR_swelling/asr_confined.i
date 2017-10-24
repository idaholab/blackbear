# @Requirement F3.50
# @Requirement F3.60

[Problem]
   coord_type = RZ
[]

[Mesh]
  file = mesh_contact_strip.e
#  uniform_refine = 1
  displacements = 'disp_x disp_y'
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]


[Variables]
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]

  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]

[]

[AuxVariables]
  [./T]
    order = FIRST
    family = LAGRANGE
    initial_condition = 35.0
  [../]

  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]

  [./ASR_ex]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]

  [./ASR_vstrain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ASR_strain_xx]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./ASR_strain_yy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./ASR_strain_zz]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./ASR_strain_xy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./ASR_strain_yz]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./ASR_strain_zx]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]

  [./total_strain_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./total_strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./total_strain_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[SolidMechanics]
  [./solid]
    disp_r = disp_x
    disp_z = disp_y
  [../]
[]

[Contact]
  [./leftright]
    system = Constraint
    master = 6
    slave = 5
    disp_x = disp_x
    disp_y = disp_y
    model = frictionless
    tangential_tolerance = 5e-4
    formulation = default
    penalty = 1.0e12
    normalize_penalty = true
  [../]
[]

[AuxKernels]
  [./ASR_ex]
    type = MaterialRealAux
    variable = ASR_ex
    block = 1
    property = ASR_extent
    execute_on = 'timestep_end'
  [../]
  [./ASR_vstrain]
    type = MaterialRealAux
    block = 1
    variable = ASR_vstrain
    property = ASR_vol_strain
    execute_on = 'timestep_end'
  [../]

  [./stress_xx]
    type = MaterialTensorAux
    tensor = stress
    variable = stress_xx
    index = 0
    execute_on = 'timestep_end'
  [../]
  [./stress_yy]
    type = MaterialTensorAux
    tensor = stress
    variable = stress_yy
    index = 1
    execute_on = 'timestep_end'
  [../]
  [./stress_zz]
    type = MaterialTensorAux
    tensor = stress
    variable = stress_zz
    index = 2
    execute_on = 'timestep_end'
  [../]
  [./stress_xy]
    type = MaterialTensorAux
    tensor = stress
    variable = stress_xy
    index = 3
    execute_on = 'timestep_end'
  [../]
  [./stress_yz]
    type = MaterialTensorAux
    tensor = stress
    variable = stress_yz
    index = 4
    execute_on = 'timestep_end'
  [../]
  [./stress_zx]
    type = MaterialTensorAux
    tensor = stress
    variable = stress_zx
    index = 5
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_xx]
    type = MaterialTensorAux
    block = 1
    tensor = ASR_strain
    variable = ASR_strain_xx
    index = 0
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_yy]
    type = MaterialTensorAux
    block = 1
    tensor = ASR_strain
    variable = ASR_strain_yy
    index = 1
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_zz]
    type = MaterialTensorAux
    block = 1
    tensor = ASR_strain
    variable = ASR_strain_zz
    index = 2
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_xy]
    type = MaterialTensorAux
    block = 1
    tensor = ASR_strain
    variable = ASR_strain_xy
    index = 3
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_yz]
    type = MaterialTensorAux
    block = 1
    tensor = ASR_strain
    variable = ASR_strain_yz
    index = 4
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_zx]
    type = MaterialTensorAux
    block = 1
    tensor = ASR_strain
    variable = ASR_strain_zx
    index = 5
    execute_on = 'timestep_end'
  [../]

  [./total_strain_zz]
    type = MaterialTensorAux
    tensor = total_strain
    variable = total_strain_zz
    index = 2
    execute_on = 'timestep_end'
  [../]

  [./total_strain_yy]
    type = MaterialTensorAux
    tensor = total_strain
    variable = total_strain_yy
    index = 1
    execute_on = 'timestep_end'
  [../]

  [./total_strain_xx]
    type = MaterialTensorAux
    tensor = total_strain
    variable = total_strain_xx
    index = 0
    execute_on = 'timestep_end'
  [../]

[]


[Materials]
  [./concreteTH]
    type = PorousMediaBase
 # setup thermal property models and parameters
 # options available: CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004 KIM-2003
    thermal_conductivity_model = CONSTANT
    thermal_capacity_model     = CONSTANT
#    aggregate_type = Siliceous               #options: Siliceous Carbonate

    ref_density_of_concrete = 2250.0         # in kg/m^3
    ref_specific_heat_of_concrete = 1100.0   # in J/(Kg.0C)
    ref_thermal_conductivity_of_concrete = 3 # in W/(m.0C)


# # setup moisture capacity and humidity diffusivity models
#     aggregate_pore_type = dense              #options: dense porous
#     aggregate_mass = 1877.0                  #mass of aggregate (kg) per m^3 of concrete
#     cement_type = 1                          #options: 1 2 3 4
#     cement_mass = 354.0                      #mass of cement (kg) per m^3 of concrete
#     water_to_cement_ratio       = 0.43
#     concrete_cure_time          = 23.0       #curing time in (days)

# #options available for humidity diffusivity:
#     moisture_diffusivity_model = Bazant      #options: Bazant Xi Mensi
#     D1 = 3.0e-12
#     aggregate_vol_fraction = 0.7             #used in Xi's moisture diffusivity model

#     coupled_moisture_diffusivity_factor = 1.0e-3  # factor for mositure diffusivity due to heat

# #coupled nonlinear variables
#     relative_humidity = rh

    temperature = T
    block = '1 2'
  [../]

  [./Elastic_concrete]
    type = SolidModel
    block = 1
    formulation = NonlinearRZ
    disp_r = disp_x
    disp_z = disp_y

    youngs_modulus = 37.3e9
    poissons_ratio = 0.22

    temp = T
    thermal_expansion = 1e-5
    stress_free_temperature = 35.0

    constitutive_model = ASR_Elastic
  [../]

  [./Elastic_steel]
    type = Elastic
    block = 2
    formulation = NonlinearRZ
    disp_r = disp_x
    disp_z = disp_y

    youngs_modulus = 193e9
    poissons_ratio = 0.3

    temp = T
    thermal_expansion = 1e-5
    stress_free_temperature = 35.0
  [../]

  [./ASR_Elastic]
    type = ConcreteElasticASRModel
    block = 1
    ASR_dependent_E = true

    youngs_modulus = 37.3e9
    poissons_ratio = 0.22
    beta_E = 0.5
  [../]

  [./ASR_swelling]
    type = VSwellingASR
    block = 1

    ASR_formulation = anisotropic

#   parameters related to ASR kinetics
    ASR_T0   = 35.0
#     ASR_vol_expansion = 0.00363
#     tau_c_T0 = 82.9
#     tau_L_T0 = 146.5
     ASR_vol_expansion = 0.00262
     tau_c_T0 = 68.9
     tau_L_T0 = 111.0

    Uc       = 5400.0
    UL       = 9400.0

# parameters related to ASR expansion reduction under tensile/compress stress state
    gamma_tensile  = 0.5
    gamma_residual = 0.5

    f_compress = -31.0e6
    f_tensile  =   3.2e6
    f_u        =  -8.0e6

     beta = 0.5
     alpha = 2.3333

#     beta = 1.5
#     alpha = 4.3333

    ASR_dependent_tensile_strength = true
    beta_f_tensile = 0.5

    temp = T

# parameters related to sub-Newton iteration for solving ASR kinetcis
    max_its = 30
    relative_tolerance = 1.0e-5
    absolute_tolerance = 1.0e-15
    output_iteration_info_on_error = true

  [../]
[]


[BCs]
  [./x_disp]
    type = DirichletBC
    variable = disp_x
    boundary = 1
    value    = 0.0
  [../]
  [./y_disp]
    type = DirichletBC
    variable = disp_y
    boundary = 3
    value    = 0.0
  [../]

  [./axial_load]
    type = NeumannBC
    variable = disp_y
    boundary = 4
    value    = -20e6
  [../]

[]

[Executioner]
  type       = Transient
  solve_type = 'PJFNK'

  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart -snes_ls -pc_hypre_boomeramg_strong_threshold'
  petsc_options_value = 'hypre boomeramg 201 cubic 0.7'
#  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package -ksp_gmres_restart'
#  petsc_options_value = 'lu       superlu_dist                  101'
#  petsc_options_iname = '-pc_type -ksp_gmres_restart'
#  petsc_options_value = 'lu       101'

#   dtmax = 86400.0       # 1 day max time step size
#   end_time = 34560000   # 400 days
    dt = 100
    num_steps = 5

#    [./TimeStepper]
#      type = SolutionTimeAdaptiveDT
#      dt = 100.0
#    [../]

#   [./TimeStepper]
#     type = IterationAdaptiveDT
#     dt = 1.0
#     optimal_iterations = 7
#     iteration_window = 1
#   [../]

  l_max_its  = 50
  l_tol      = 1e-6
  nl_max_its = 10
  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-6
[]

[Outputs]
  file_base      = asr_confined_strip_out
  interval       = 1
  exodus         = true
  [./Console]
    type = Console
    perf_log = true
  [../]
[]

