# SpecifiedVaporPressureBC

!syntax description /BCs/SpecifiedVaporPressureBC

## Description

This boundary condition provides the ability to prescribe the vapor pressure at a location to influence the solution for the relative humidity in a concrete moisture transport simulation. It computes a saturated vapor pressure, $P_{vs}$, defined by [!cite](Bary2012) as:

!equation id=sat_vap_pres
P_{vs} = P_{atm}\exp\left(4871.3\frac{T-100}{373.15T}\right)

where

$T$ = temperature in K\\
$P_{atm}$ = standard atmospheric pressure (101325 Pa)

Note that although $T$ in the above formula is in K, the model is designed to accept T in units of $\degree$C and convert to K internally.  This saturated vapor pressure is then used to compute a prescribed relative humidity, $P_{v,pres}$, which is prescribed using a Dirichlet-style boundary condition:

!equation id=rh_pres
rh_{pres}=P_{v,pres}/P_{vs}

To avoid sudden changes, this is ramped from initial prescribed conditions over a user-specified duration.

!syntax parameters /BCs/SpecifiedVaporPressureBC

!syntax inputs /BCs/SpecifiedVaporPressureBC

!syntax children /BCs/SpecifiedVaporPressureBC

!bibtex bibliography
