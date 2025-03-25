# Test for small strain Euler beam axial loading in x direction.

# Modeling a pipe with an OD of 10 inches and ID of 8 inches
# The length of the pipe is 5 feet (60 inches) and E = 30e6
# G = 11.5384615385e6 with nu = 0.3
# The applied axial load is 50000 lb which results in a
# displacement of 3.537e-3 inches at the end, i.e,  0.005894697 extension

# delta = PL/AE = 50000 * 60 / pi (5^2 - 4^2) * 30e6 = 3.537e-3

# In this analysis the applied force is used as a BC

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 10
  xmin = 0.0
  xmax = 60.0
  displacements = 'disp_x disp_y disp_z'
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
  [./disp_z]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_z]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[BCs]
  [./fixx1]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./fixy1]
    type = DirichletBC
    variable = disp_y
    boundary = left
    value = 0.0
  [../]
  [./fixz1]
    type = DirichletBC
    variable = disp_z
    boundary = left
    value = 0.0
  [../]
  [./fixr1]
    type = DirichletBC
    variable = rot_x
    boundary = left
    value = 0.0
  [../]
  [./fixr2]
    type = DirichletBC
    variable = rot_y
    boundary = left
    value = 0.0
  [../]
  [./fixr3]
    type = DirichletBC
    variable = rot_z
    boundary = left
    value = 0.0
  [../]
[]

[NodalKernels]
  [./force_x2]
    type = ConstantRate
    variable = disp_x
    boundary = right
    rate = 50000.0
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]
[Executioner]
  type = Transient
  solve_type = PJFNK
  line_search = 'none'
  nl_max_its = 15
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-8

  dt = 1
  dtmin = 1
  end_time = 1
[]

[Kernels]
  [./solid_disp_x]
    type = StressDivergenceBeam
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 0
    variable = disp_x
  [../]
  [./solid_disp_y]
    type = StressDivergenceBeam
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 1
    variable = disp_y
  [../]
  [./solid_disp_z]
    type = StressDivergenceBeam
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 2
    variable = disp_z
  [../]
  [./solid_rot_x]
    type = StressDivergenceBeam
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 3
    variable = rot_x
  [../]
  [./solid_rot_y]
    type = StressDivergenceBeam
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 4
    variable = rot_y
  [../]
  [./solid_rot_z]
    type = StressDivergenceBeam
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 5
    variable = rot_z
  [../]
[]

[AuxVariables]
  [forces_x]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [forces_x]
    type = MaterialRealVectorValueAux
    property = forces
    variable = forces_x
    execute_on = timestep_end
  []
[]

[Materials]
  [./elasticity]
    type = ComputeElasticityBeam
    shear_coefficient = 1.0
    youngs_modulus = 30e6
    poissons_ratio = 0.3
    block = 0
  [../]
  [./strain]
    type = ComputeIncrementalBeamStrain
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    area = 28.274
    Ay = 0.0
    Az = 0.0
    Iy = 1.0
    Iz = 1.0
    y_orientation = '0.0 1.0 0.0'
  [../]
  [./stress]
    type = ComputeBeamResultants
    block = 0
  [../]
[]

[Postprocessors]
  [./disp_x60]
    type = PointValue
    point = '60.0 0.0 0.0'
    variable = disp_x
  [../]
  [./disp_y60]
    type = PointValue
    point = '60.0 0.0 0.0'
    variable = disp_y
  [../]
  [./disp_x30]
    type = PointValue
    point = '30.0 0.0 0.0'
    variable = disp_x
  [../]
  [./disp_y30]
    type = PointValue
    point = '30.0 0.0 0.0'
    variable = disp_y
  [../]
  [./disp_x0]
    type = PointValue
    point = '0.0 0.0 0.0'
    variable = disp_x
  [../]
  [./disp_y0]
    type = PointValue
    point = '0.0 0.0 0.0'
    variable = disp_y
  [../]
  [./avg_extension_raio]
    type = AverageExtensionRatio
    first_point = '60.0 0.0 0.0
                   30.0 0.0 0.0'
    last_point  = '30.0 0.0 0.0
                    0.0 0.0 0.0'
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./forces_x]
    type = PointValue
    point = '60.0 0.0 0.0'
    variable = forces_x
  [../]
[]

[Outputs]
  csv = true
[]
