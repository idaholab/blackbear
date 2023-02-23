# Compute Multiple Inelastic Damage Stress

!syntax description /Materials/ComputeMultipleInelasticDamageStress

## Description

`ComputeMultipleInelasticDamageStress` is a class based on `ComputeMultipleInelasticStress`
that is designed specifically to be used with [DamagePlasticityStressUpdate](DamagePlasticityStressUpdate.md).
The main difference between this class and the standard `ComputeMultipleInelasticStress` class is that
it makes some modifications to both the current and old stress to account for the damage in a way that
is appropriate for the damaged plasticity model. It divides the old stress passed to the plasticity model
by $(1-D)$, and multiplies the new stress by $(1-D)$, where $D$ is the damage index.

This ComputeMultipleInelasticStress is to be used with (/DamagePlasticityStressUpdate.md).
## Example Input Files

The input settings for the inelastic material model is as follows

!listing test/tests/damage_plasticity_model/uniaxial_test.i block=Materials/stress

!syntax parameters /Materials/ComputeMultipleInelasticDamageStress

!syntax inputs /Materials/ComputeMultipleInelasticDamageStress

!syntax children /Materials/ComputeMultipleInelasticDamageStress

!bibtex bibliography
