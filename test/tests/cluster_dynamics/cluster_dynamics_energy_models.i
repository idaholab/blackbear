[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 1
[]

[Problem]
  # Required for the intra-variable Jacobian; keep the fixed sparsity after the first assembly
  use_hash_table_matrix_assembly = true
  restore_original_nonzero_pattern = false
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
  # Each binding-energy model is in its own block; the tests select one with the active list
  active = 'clusters_dot interfacial_energy'
  [clusters_dot]
    type = ArrayTimeDerivativeNodalKernel
    variable = clusters
  []
  [interfacial_energy]
    type = ClusterDynamicsNodalKernel
    variable = clusters
    generation = 1.0e-3
    sink = 0.1
    rate_model = interfacial_energy
    monomer_diffusivity = 1.0e-21
    temperature = 600
    atomic_volume = 1.1782924e-29
    binding_energy_model = interfacial_energy
    sigma = 0.37
    Omega_kB_K = 6255
    DeltaS_kB = 0.866
  []
  [capillary_interfacial]
    # E_b(n) = (H - T*S)*k_B - (36*pi)^(1/3)*V_at^(2/3)*sigma*[n^p - (n - 1)^p]
    type = ClusterDynamicsNodalKernel
    variable = clusters
    generation = 1.0e-3
    sink = 0.1
    rate_model = interfacial_energy
    monomer_diffusivity = 1.0e-21
    temperature = 600
    atomic_volume = 1.1782924e-29
    binding_energy_model = capillary
    binding_energy_enthalpy_kB_K = 6255
    binding_energy_entropy_kB = 0.866
    sigma = 0.37
    binding_energy_exponent = 0.6666666666666666
  []
  [capillary_direct]
    # E_b(n) = 0.55 - 0.40*[n^(2/3) - (n - 1)^(2/3)]
    type = ClusterDynamicsNodalKernel
    variable = clusters
    generation = 1.0e-3
    sink = 0.1
    rate_model = interfacial_energy
    monomer_diffusivity = 1.0e-21
    temperature = 600
    atomic_volume = 1.1782924e-29
    binding_energy_model = capillary
    binding_energy_constant_eV = 0.55
    binding_energy_coefficient_eV = 0.40
    binding_energy_exponent = 0.6666666666666666
  []
  [binding_energy_table]
    type = ClusterDynamicsNodalKernel
    variable = clusters
    generation = 1.0e-3
    sink = 0.1
    rate_model = interfacial_energy
    monomer_diffusivity = 1.0e-21
    temperature = 600
    atomic_volume = 1.1782924e-29
    binding_energy_model = binding_energy_table
    # E_b(2), E_b(3), ..., E_b(8)
    binding_energy_table_eV = '0.25 0.30 0.34 0.37 0.39 0.41 0.43'
  []
  [formation_energy_table]
    type = ClusterDynamicsNodalKernel
    variable = clusters
    generation = 1.0e-3
    sink = 0.1
    rate_model = interfacial_energy
    monomer_diffusivity = 1.0e-21
    temperature = 600
    atomic_volume = 1.1782924e-29
    binding_energy_model = formation_energy_table
    # E_f(1), E_f(2), ..., E_f(8)
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

[Preconditioning]
  # Avoid the default full coupling matrix, which is O(N^2) in the number of array components
  [smp]
    type = SMP
    full = false
  []
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  # Reorder so that factoring the dense monomer row and column does not cost O(N^2)
  petsc_options_iname = '-pc_type -pc_factor_mat_ordering_type'
  petsc_options_value = 'ilu      rcm'
  nl_rel_tol = 1.0e-9
  nl_abs_tol = 1.0e-12
  dt = 0.01
  num_steps = 3
[]

[Outputs]
  csv = true
[]
