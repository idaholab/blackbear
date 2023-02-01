# Compute Multiple Inelastic Damage Stress

!syntax description /Materials/ComputeMultipleInelasticDamageStress

## Description

This class `ComputeMultipleInelasticDamageStress` computes the stress with the damage obtained from 
  the previous time step. This is done mainly to avoid convergence problems

This ComputeMultipleInelasticStress is to be used with (/DamagePlasticityStressUpdate.md).
## Example Input Files

The input settings for the inelastic material model is as follows

!listing test/tests/damage_plasticity_model/uniaxial_test.i block=Materials/stress

!syntax parameters /Materials/ComputeMultipleInelasticDamageStress

!syntax inputs /Materials/ComputeMultipleInelasticDamageStress

!syntax children /Materials/ComputeMultipleInelasticDamageStress

!bibtex bibliography
