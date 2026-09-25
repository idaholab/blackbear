[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 1
[]

[Variables]
  [clusters]
    family = LAGRANGE
    order = FIRST
    components = 8
  []
[]

[Functions]
  [index_fn]
    type = PiecewiseConstant
    x = '0 0.9'
    y = '0.01 0.001'
    direction = left
  []
[]

[ICs]
  [clusters_ic]
    type = ArrayFunctionIndexIC
    variable = clusters
    function = index_fn
  []
[]

[NodalKernels]
  [clusters_dot]
    type = ArrayTimeDerivativeNodalKernel
    variable = clusters
  []
  [clusters_dyn]
    type = ADClusterDynamicsNodalKernel
    variable = clusters
    generation = 1.0e-3
    sink = 0.1
    rate_model = interfacial_energy
    diffusivity_model = constant
    monomer_diffusivity = 1.0e-21
    temperature = 600
    atomic_volume = 1.1782924e-29
    sigma = 0.37
    Omega_kB_K = 6255
    DeltaS_kB = 0.866
    binding_energy_enthalpy_kB_K = 6255
    binding_energy_entropy_kB = 0.866
    binding_energy_table_eV = '0.25 0.30 0.34 0.37 0.39 0.41 0.43'
    formation_energy_table_eV = '1.60 2.70 3.60 4.40 5.10 5.75 6.35 6.90'
  []
[]

[Postprocessors]
  [avg_cluster_radius]
    type = ClusterAverageRadius
    clusters = clusters
    r1 = 1.0
    n_minimum = 2
  []
  [monomer_concentration]
    type = ClusterSizeConcentration
    clusters = clusters
    n_size = 1
  []
  [total_cluster_density]
    type = ClusterTotalDensity
    clusters = clusters
    n_minimum = 1
  []
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  nl_rel_tol = 1.0e-9
  nl_abs_tol = 1.0e-12
  dt = 0.01
  num_steps = 3
[]

[Outputs]
  csv = true
[]
