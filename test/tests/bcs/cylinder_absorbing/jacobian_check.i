# Jacobian check for CylinderAbsorbingBC (non-AD hand-coded Jacobian)
#
# Verifies that computeQpJacobian and computeQpOffDiagJacobian match a PETSc
# finite-difference Jacobian.  The off-diagonal entries encode the (n . v)
# projection coupling between displacement components; dropping them would
# cause this check to fail.
#
# Tiny 3D slab -- minimal mesh that exercises all three displacement components
# and their cross-coupling through the boundary normal at the top face.
#
# The base slab is rotated 30 degrees about the z-axis so the "top" face
# normal becomes (-sin30, cos30, 0) ~ (-0.5, 0.866, 0).  Both n_x and n_y are
# nonzero, so the off-diagonal Jacobian contribution -rho*(c_p-c_s)*n_j*n_k
# is nontrivial; with an axis-aligned normal the off-diagonal would be
# identically zero and a bug there would slip through.

[Mesh]
  [base]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 1
    ny = 2
    nz = 1
    xmin = 0.0
    xmax = 0.5
    ymin = 0.0
    ymax = 1.0
    zmin = 0.0
    zmax = 0.5
  []
  [rotate]
    type = TransformGenerator
    input = base
    transform = ROTATE
    vector_value = '30 0 0'
  []
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Physics/SolidMechanics/Dynamic]
  [all]
    add_variables = true
    newmark_beta = 0.25
    newmark_gamma = 0.5
    strain = SMALL
    density = density
  []
[]

[BCs]
  # Fix lateral and bottom faces to constrain rigid-body modes
  [fix_x_sides]
    type = DirichletBC
    variable = disp_x
    boundary = 'left right front back bottom'
    value = 0.0
  []
  [fix_y_bottom]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  []
  [fix_z_sides]
    type = DirichletBC
    variable = disp_z
    boundary = 'left right front back bottom'
    value = 0.0
  []

  # CylinderAbsorbingBC on the top face -- one instance per component.
  # After the 30-degree z-axis rotation the outward normal is approximately
  # (-0.5, 0.866, 0); the off-diagonal Jacobian entries -rho*(c_p-c_s)*n_j*n_k
  # are nonzero between disp_x and disp_y, exercising the projection coupling
  # the hand-coded computeQpOffDiagJacobian was written for.
  [absorb_x_top]
    type = CylinderAbsorbingBC
    variable = disp_x
    boundary = top
    displacements = 'disp_x disp_y disp_z'
    density = density
    shear_modulus = shear_modulus
    lambda = lambda
    component = x
  []
  [absorb_y_top]
    type = CylinderAbsorbingBC
    variable = disp_y
    boundary = top
    displacements = 'disp_x disp_y disp_z'
    density = density
    shear_modulus = shear_modulus
    lambda = lambda
    component = y
  []
  [absorb_z_top]
    type = CylinderAbsorbingBC
    variable = disp_z
    boundary = top
    displacements = 'disp_x disp_y disp_z'
    density = density
    shear_modulus = shear_modulus
    lambda = lambda
    component = z
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 4.0e9
    poissons_ratio = 0.25
  []
  [stress]
    type = ComputeLinearElasticStress
  []
  [density_mat]
    type = GenericConstantMaterial
    prop_names = 'density'
    prop_values = '2000.0'
  []
  # Lame parameters for the BC (E=4e9, nu=0.25 -> lambda = mu = 1.6e9 Pa)
  [lame_params]
    type = GenericConstantMaterial
    prop_names = 'shear_modulus lambda'
    prop_values = '1.6e9 1.6e9'
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
  solve_type = NEWTON
  start_time = 1.0e-4
  num_steps = 1
  dt = 1.0e-4
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-14
[]

[Outputs]
  exodus = false
[]
