# BatchStressGrad

## Description

This `UserObject` computes double contraction of the elastic tensor derivative and the forward mechanical strain, i.e.,
\begin{equation}
\underbrace{\frac{\partial \boldsymbol{C}}{\partial p}}_{\text{elastic tensor derivative}} \underbrace{(\boldsymbol{L} u)}_{\text{forward strain}}
\end{equation}
as a batch material.
Here, the $\boldsymbol{C}$ is the elastic tensor, $p$ is the interested parameter, $\boldsymbol{L}$ is the differential operator for elasticity problem, and $\boldsymbol{L} u$ is strain from the forward problem. This object requires the elastic tensor derivative material property (i.e., $\frac{\partial \boldsymbol{C}}{\partial p}$) as its input.

This object is used together with [AdjointStrainStressGradInnerProduct](/AdjointStrainStressGradInnerProduct.md) in a elastic inversion problem.

## Example Input File Syntax

!listing test/tests/bimaterial_elastic_inversion_batch_mat/grad.i block=UserObjects/stress_grad_lambda

!syntax parameters /UserObjects/BatchStressGrad

!syntax inputs /UserObjects/BatchStressGrad

!syntax children /UserObjects/BatchStressGrad
