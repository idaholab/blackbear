# AdjointStrainStressGradInnerProduct

!syntax description /VectorPostprocessors/AdjointStrainStressGradInnerProduct

## Description

This `VectorPostprocessor` is utilized in a mechanical inverse optimization problem (see [Inverse Optimization Theory](/InvOptTheory.md)) for computing the gradient of objective function with respect to a parameter of interest.

Specifically, the objective function is defined as the sum of the regularization and the displacement mismatch (between the simulation and the measurement data at specific locations) as follows:
\begin{equation}
  F({u}, \boldsymbol{p}) = ||\boldsymbol{S}{u} - \boldsymbol{u}_m ||^2 + R({u}, \boldsymbol{p}),
\end{equation}
where ${u}$ denotes the displacement from the forward problem, $\boldsymbol{S}$ is the sampling operator, $\boldsymbol{u}_m$ are the measurements, $\boldsymbol{p} = [p_1, p_2, ..., p_n]$ is the vector of parameters,  and $R$ is the regularization term.

The gradient of objective function with respect to one scalar parameter ($p_i$) is
\begin{equation}
  \frac{\text{d}{F}}{\text{d}p_i} = - \int \underbrace{(\boldsymbol{L}\lambda)^T}_{\text{adjoint strain}}  \underbrace{\frac{\partial \boldsymbol{C}}{\partial p_i} (\boldsymbol{L} u)}_{\text{gradient of stress}} \text{d}\Omega,
\end{equation}
where ${F}$ is the objective function, $\boldsymbol{L}$ is the symmetric strain operator for the elasticity problem, $\lambda$ is the adjoint displacement, $\boldsymbol{C}$ is the elasticity tensor, and $p_i$ is the interested scalar parameter.

Specifically, this object takes the `adjoint strain` and the `gradient of stress w.r.t. parameter` as inputs, computes the double contraction of the two inputs, and integrate over the entire domain. Specifically, the `adjoint strain` is the strain from the adjoint problem, the `gradient of stress w.r.t. parameter` should be a batch material (e.g., [BatchStressGrad](/BatchStressGrad.md)).

## Example Input File Syntax

!listing test/tests/bimaterial_elastic_inversion_batch_mat/grad.i block=VectorPostprocessors/grad_lambda

!syntax parameters /VectorPostprocessors/AdjointStrainStressGradInnerProduct

!syntax inputs /VectorPostprocessors/AdjointStrainStressGradInnerProduct

!syntax children /VectorPostprocessors/AdjointStrainStressGradInnerProduct
