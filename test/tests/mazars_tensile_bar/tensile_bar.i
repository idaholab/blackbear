# This tests a bar pulled in tension from one side.
# The material is modeled using the MazarsDamage model,
# and a single band of elements is given a slightly lower
# tensile strength to force damage localization.

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 2
  nz = 2
  xmax = 0.05
  ymax = 0.01
  zmax = 0.01
  elem_type = HEX8
[]

[AuxVariables]
  [react_x]
  []
  [react_y]
  []
  [react_z]
  []
  [tensile_strength]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Modules/TensorMechanics/Master]
  [all]
    strain = FINITE
    incremental = true
    add_variables = true
    generate_output = 'stress_xx strain_xx'
    save_in = 'react_x react_y react_z'
  []
[]

[AuxKernels]
  [damage_index]
    type = MaterialRealAux
    variable = damage_index
    property = damage_index
    execute_on = timestep_end
  []
  [tensile_strength]
    type = FunctionAux
    variable = tensile_strength
    function = 'if(x<0.005,0.99e6,1.0e6)'
    execute_on = initial
  []
[]

[BCs]
  [symmy]
    type = PresetBC
    variable = disp_y
    boundary = bottom
    value = 0
  []
  [symmx]
    type = PresetBC
    variable = disp_x
    boundary = left
    value = 0
  []
  [symmz]
    type = PresetBC
    variable = disp_z
    boundary = back
    value = 0
  []
  [axial_load]
    type = FunctionDirichletBC
    preset = true
    variable = disp_x
    boundary = right
    function = 't'
  []
[]

[Materials]
  [damage]
    type = MazarsDamage
    tensile_strength = tensile_strength
    a_t = 0.87
    a_c = 0.65
    b_t = 20000
    b_c = 2150
    use_old_damage = true
    outputs = exodus
    output_properties = damage_index
  []
  [stress]
    type = ComputeDamageStress
    damage_model = damage
  []
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    poissons_ratio = 0.2
    youngs_modulus = 10e9
  []
[]

[Postprocessors]
  [react]
    type = SideAverageValue
    variable = react_x
    boundary = right
  []
  [disp]
    type = SideAverageValue
    variable = disp_x
    boundary = right
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient

  l_max_its  = 50
  l_tol      = 1e-8
  nl_max_its = 20
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-8

  dt = 5e-7
  dtmin = 5e-7
  end_time = 1e-5
[]

[Outputs]
  exodus = true
  csv = true
[]
