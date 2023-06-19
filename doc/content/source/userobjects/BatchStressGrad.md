# BatchStressGrad

## Description

This `UserObject` computes double contraction of the elasticity tensor derivative and the forward mechanical strain, i.e.,
\begin{equation}
\underbrace{\frac{\partial \boldsymbol{C}}{\partial p}}_{\text{elasticity tensor derivative}} \underbrace{(\boldsymbol{L} u)}_{\text{forward strain}}
\end{equation}
as a batch material.
Here, $\boldsymbol{C}$ is the elasticity tensor, $p$ is the interested parameter, $\boldsymbol{L}$ is the symmetric strain operator for the elasticity problem, and $\boldsymbol{L} u$ is the strain from the forward problem. This object requires the elasticity tensor derivative material property (i.e., $\frac{\partial \boldsymbol{C}}{\partial p}$) as its input.

This object is used together with [AdjointStrainStressGradInnerProduct](/AdjointStrainStressGradInnerProduct.md) in a linear elastic inversion problem.

## Example Input File Syntax

!listing test/tests/bimaterial_elastic_inversion_batch_mat/grad.i block=UserObjects/stress_grad_lambda

!syntax parameters /UserObjects/BatchStressGrad

!syntax inputs /UserObjects/BatchStressGrad

!syntax children /UserObjects/BatchStressGrad
