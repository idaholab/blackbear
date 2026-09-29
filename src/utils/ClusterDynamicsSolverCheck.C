/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                       BlackBear                              */
/*                                                              */
/*           (c) 2017 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "ClusterDynamicsSolverCheck.h"

#include "FEProblemBase.h"
#include "MooseObject.h"
#include "NonlinearSystemBase.h"

#include "libmesh/coupling_matrix.h"

#include <petscsnes.h>

namespace
{
/// Value of the PETSc option name under prefix in the options database, or fallback if unset
std::string
petscOption(::PetscOptions options,
            const std::string & prefix,
            const std::string & name,
            const std::string & fallback)
{
  char value[256];
  PetscBool set = PETSC_FALSE;
  LibmeshPetscCallA(
      PETSC_COMM_SELF,
      PetscOptionsGetString(options, prefix.c_str(), name.c_str(), value, sizeof(value), &set));
  return set ? std::string(value) : fallback;
}
} // namespace

namespace ClusterDynamics
{
void
checkSolverSetup(const MooseObject & object,
                 const FEProblemBase & problem,
                 unsigned int nl_sys_num,
                 unsigned int first_component,
                 unsigned int n_components)
{
  if (problem.useHashTableMatrixAssembly() && problem.restoreOriginalNonzeroPattern())
    object.mooseWarning(
        "Set Problem/restore_original_nonzero_pattern = false. With hash table matrix assembly, "
        "MOOSE otherwise rebuilds the matrix from the hash table for every Jacobian, which "
        "costs time proportional to the square of the number of cluster sizes because of the "
        "dense monomer row. The cluster-dynamics sparsity pattern does not change, so the "
        "pattern from the first assembly can be kept.");

  if (problem.ignoreZerosInJacobian() && !problem.restoreOriginalNonzeroPattern())
    object.mooseWarning(
        "Set Problem/ignore_zeros_in_jacobian = false. Jacobian entries that are zero at the "
        "first assembly, such as those of cluster sizes with a zero initial concentration, are "
        "otherwise left out of the sparsity pattern that is kept for later Jacobians, and PETSc "
        "must reallocate the matrix when they become nonzero.");

  const auto * cm = problem.couplingMatrix(nl_sys_num);
  if (cm && n_components > 1 && (*cm)(first_component, first_component + 1))
    object.mooseWarning(
        "The preconditioner couples the array components of the cluster variable with each "
        "other. This is the default when there is no Preconditioning block (an SMP "
        "preconditioner with full = true), and building and traversing that coupling costs "
        "time proportional to the square of the number of cluster sizes. The kernel adds its "
        "Jacobian entries directly, so use an SMP preconditioner with full = false:\n"
        "  [Preconditioning]\n    [smp]\n      type = SMP\n      full = false\n    []\n  []");
}

bool
checkFactorOrdering(const MooseObject & object, NonlinearSystemBase & nl)
{
  SNES snes = nl.getSNES();
  if (!snes)
    return false;

  KSP ksp;
  PC pc;
  PCType pc_type;
  LibmeshPetscCallA(PETSC_COMM_SELF, SNESGetKSP(snes, &ksp));
  LibmeshPetscCallA(PETSC_COMM_SELF, KSPGetPC(ksp, &pc));
  LibmeshPetscCallA(PETSC_COMM_SELF, PCGetType(pc, &pc_type));
  if (!pc_type)
    return false;

  ::PetscOptions options;
  const char * pc_prefix;
  LibmeshPetscCallA(PETSC_COMM_SELF, PetscObjectGetOptions((PetscObject)pc, &options));
  LibmeshPetscCallA(PETSC_COMM_SELF, PCGetOptionsPrefix(pc, &pc_prefix));
  std::string prefix = pc_prefix ? pc_prefix : "";
  std::string type = pc_type;

  // Block preconditioners factor each block with a sub preconditioner, ILU by default
  if (type == PCBJACOBI || type == PCASM || type == PCGASM)
  {
    type = petscOption(options, prefix, "-sub_pc_type", PCILU);
    prefix += "sub_";
  }

  const bool complete = type == PCLU || type == PCCHOLESKY;
  const bool incomplete = type == PCILU || type == PCICC;
  if (!complete && !incomplete)
    return true;

  // External factorization packages choose their own ordering
  if (petscOption(options, prefix, "-pc_factor_mat_solver_type", MATSOLVERPETSC) != MATSOLVERPETSC)
    return true;

  // PETSc orders complete factorizations with nested dissection and incomplete ones naturally
  const auto ordering = petscOption(options,
                                    prefix,
                                    "-pc_factor_mat_ordering_type",
                                    complete ? MATORDERINGND : MATORDERINGNATURAL);
  if (ordering != MATORDERINGNATURAL)
    return true;

  const std::string option = "-" + prefix + "pc_factor_mat_ordering_type";
  const std::string advice =
      "Set the PETSc option " + option +
      " to 'nd' or 'rcm', for example with petsc_options_iname = '-pc_factor_mat_ordering_type' "
      "and petsc_options_value = 'rcm' in the Executioner block.";
  if (complete)
    object.mooseError("The cluster-dynamics Jacobian has a dense monomer row and column. Its '",
                      type,
                      "' factorization with the natural ordering fills in to a dense matrix, "
                      "which needs memory proportional to the square of the number of cluster "
                      "sizes. ",
                      advice);
  else
  {
    const bool has_fill = petscOption(options, prefix, "-pc_factor_levels", "0") != "0";
    object.mooseWarning("The cluster-dynamics Jacobian has a dense monomer row and column. Its '",
                        type,
                        "' factorization with the natural ordering, which is the PETSc default "
                        "for incomplete factorizations, costs time",
                        has_fill ? " and memory" : "",
                        " proportional to the square of the number of cluster sizes. ",
                        advice);
  }
  return true;
}
} // namespace ClusterDynamics
