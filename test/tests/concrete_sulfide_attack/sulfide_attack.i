[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = 0.0
  ymin = 0.0
  xmax = 0.5
  ymax = 0.001
  nx = 100
  ny = 1
[]

[ReactionNetwork]
  primary_aqueous_species   = 'ca     cl     h       na     so4'
  initial_condition         = '1.0e-2 2.0e-2 1.0e-10 1.0e-5 1.0e-5'

  # aqueous speciations
  secondary_aqueous_species = 'cacl cacl2_aq caoh caso4_aq h2so4_aq hcl_aq hso4 nacl_aq naoh_aq naso4 oh'
  aqueous_speciations       = '(1.0)ca + (1.0)cl  = (1.0)cacl      -0.7
                               (1.0)ca + (2.0)cl  = (1.0)cacl2_aq  -0.653
                               (1.0)ca + (-1.0)h  = (1.0)caoh      -12.85
                               (1.0)ca + (1.0)so4 = (1.0)caso4_aq   2.1
                               (2.0)h  + (1.0)so4 = (1.0)h2so4_aq  -1.021
                               (1.0)h  + (1.0)cl  = (1.0)hcl_aq     0.7
                               (1.0)h  + (1.0)so4 = (1.0)hso4       1.976
                               (1.0)na + (1.0)cl  = (1.0)nacl_aq   -0.782
                               (1.0)na + (-1.0)h  = (1.0)naoh_aq   -14.799
                               (1.0)na + (1.0)so4 = (1.0)naso4      0.82
                               (-1.0)h             = (1.0)oh       -13.991'
  # mineral-solution reactions
  minerals = 'gypsum'
  initial_mineral_conc = '0.0'
  mineral_solution_reactions = '(1.0)ca+(1.0)so4=gypsum'
  log_k = '-6'
  specific_reactive_surface_area = '0.1'
  kinetic_rate_constant = '6.456542e-6'
  activation_energy = '1.5e4'
  gas_constant = 8.314
  reference_temperature = 298.15
  system_temperature = 298.15
[]

[AuxVariables]
  [./porosity]
    order = CONSTANT
    family = Monomial
  [../]
[]

[AuxKernels]
  [./porosity_output]
    type = MaterialRealAux
    variable = porosity
    property = porosity
  [../]
[]

[BCs]
  [./ca_left]
    type = DirichletBC
    preset = false
    variable = ca
    boundary = 3
    value = 1.0e-2
  [../]
  [./cl-_left]
    type = DirichletBC
    preset = false
    variable = cl
    boundary = 3
    value = 2.0e-2
  [../]
  [./h+_left]
    type = DirichletBC
    preset = false
    variable = h
    boundary = 3
    value = 1.0e-10
  [../]
  [./na+_left]
    type = DirichletBC
    preset = false
    variable = na
    boundary = 3
    value = 1.0e-5
  [../]
  [./so42-_left]
    type = DirichletBC
    preset = false
    variable = so4
    boundary = 3
    value = 1.0e-5
  [../]

#   [./ca_right]
#     type = DirichletBC
#     variable = ca
#     boundary = 1
#     value = 1.0e-2
#   [../]
  [./cl-_right]
    type = DirichletBC
    preset = false
    variable = cl
    boundary = 1
    value = 1.0e-5
  [../]
#   [./h+_right]
#     type = DirichletBC
#     variable = h
#     boundary = 1
#     value = 1.0e-10
#   [../]
  [./na+_right]
    type = DirichletBC
    preset = false
    variable = na
    boundary = 1
    value = 2.0e-2
  [../]
  [./so42-_right]
    type = DirichletBC
    preset = false
    variable = so4
    boundary = 1
    value = 1.0e-2
  [../]
[]

[Materials]
  [./example]
    type = PorousMediaBase
    block = 0
    initial_diffusivity      = 1.0e-9
    initial_porosity         = 0.1
    initial_permeability     = 1.0e-18
    initial_mineral_conc     = 0.00
    mineral_compositions     = gypsum
    mineral_molecular_weight = 136.1406
    mineral_density          = 2.308
  [../]
[]

[Executioner]
 type = Transient

  #Preconditioned JFNK (default)
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre    boomeramg'

  dt = 100.0
  num_steps = 5
#   dtmax = 864000.0
#   end_time = 157680000   # 5 years
#   [./TimeStepper]
#     type = SolutionTimeAdaptiveDT
#     dt = 100.0
#   [../]

  l_max_its  = 50
  l_tol      = 1e-6
  nl_max_its = 10
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-15
[]

[Outputs]
  exodus = true
[]
