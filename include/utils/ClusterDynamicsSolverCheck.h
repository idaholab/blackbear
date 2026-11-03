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

#pragma once

class FEProblemBase;
class MooseObject;
class NonlinearSystemBase;

namespace ClusterDynamics
{
/**
 * Warn about Problem and Preconditioning settings that make the setup or Jacobian assembly of a
 * cluster-dynamics array variable cost time proportional to the square of its number of
 * components: rebuilding the hash table matrix for every Jacobian, and a preconditioner coupling
 * matrix that couples the array components with each other.
 *
 * @param nl_sys_num nonlinear system number of the cluster variable
 * @param first_component variable number of the first array component
 * @param n_components number of array components
 */
void checkSolverSetup(const MooseObject & object,
                      const FEProblemBase & problem,
                      unsigned int nl_sys_num,
                      unsigned int first_component,
                      unsigned int n_components);

/**
 * Check the ordering used by the PETSc factorization that preconditions the cluster-dynamics
 * Jacobian. The Jacobian has a dense monomer row and column, so a factorization with the natural
 * ordering eliminates through that row first: a complete factorization then fills in to a dense
 * matrix (an error), and an incomplete factorization costs O(N^2) time (a warning).
 *
 * This must be called while the nonlinear system is solving, after PETSc has configured the
 * preconditioner from the options database but before it has factored the Jacobian.
 *
 * @return true if the preconditioner was configured and could be checked
 */
bool checkFactorOrdering(const MooseObject & object, NonlinearSystemBase & nl);
} // namespace ClusterDynamics
