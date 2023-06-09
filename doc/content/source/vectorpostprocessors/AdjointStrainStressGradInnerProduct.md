# AdjointStrainStressGradInnerProduct

!syntax description /VectorPostprocessors/AdjointStrainStressGradInnerProduct

## Description

This `VectorPostprocessor` computes the gradient of objective function with respect to interested parameter for elastic problem. Specifically,
\begin{equation}
  \frac{\text{d}\boldsymbol{F}}{\text{d}p} = - \int \underbrace{(\boldsymbol{L}\lambda)^T}_{\text{adjoint strain}}  \underbrace{\frac{\partial \boldsymbol{C}}{\partial p} (\boldsymbol{L} u)}_{\text{stress gradient}} \text{d}\Omega,
\end{equation}
where $\boldsymbol{F}$ is the objective function, $\boldsymbol{L}$ is the differential operator for the elasticity problem, $\lambda$ is the adjoint displacement, $\boldsymbol{C}$ is the elastic tensor, $p$ is the interested parameter, and $u$ is displacement from the forward problem.

Specifically, this object takes the `adjoint strain` and the `stress gradient` as inputs, computes the double contraction of the two inputs, and integrate over the entire domain. Specifically, the `adjoint strain` is the strain from the adjoint problem, the `stress gradient` should be a batch material (e.g., [BatchStressGrad](/BatchStressGrad.md)).

## Example Input File Syntax

!listing test/tests/bimaterial_elastic_inversion_batch_mat/grad.i block=VectorPostprocessors/grad_lambda

!syntax parameters /VectorPostprocessors/AdjointStrainStressGradInnerProduct

!syntax inputs /VectorPostprocessors/AdjointStrainStressGradInnerProduct

!syntax children /VectorPostprocessors/AdjointStrainStressGradInnerProduct
