# Units in BlackBear

## General Concepts

BlackBear is a multiphysics simulation code based on the finite-element method. Finite-element models are inherently flexible in terms of the system of units that they employ. Because of that, most of the models in the MOOSE framework and modules that BlackBear is built on are not tied to a specific unit system.

However, when defining a model, it is essential to intentionally choose a unit system, and to consistently use that across the set of physics models used in the simulation. Even though many physics models may be flexible regarding the unit system employed, their inputs and outputs---which can include input parameters, field variables, model dimensions, and residuals---are typically physical quantities that are tied to a certain type of unit. For example, finite element mesh coordinates must be defined in length units, and solution time must be in time units. 

In some cases, physics models are based on built-in data that is defined in a specific unit system. In those cases, models may specify that specific parameters or coupled variables are in a certain unit system, or they may alternatively provide the user options for using the model with multiple alternative unit systems. Many of the models in BlackBear are tied to specific units, and when a certain unit is required for a model, that is noted in the documentation for that model. Use of International System (SI) units is generally recommended with BlackBear, and most models are designed to operate with those units.

## Unit Documentation

Whenever a parameter for a model is expected to be in a specific unit or type of unit, this is documented within the information on that model's parameters. In the documentation for each parameter, there is a list of important information about that variable, and the unit is included in that list.

When the model requires only that a type of unit is used, but allows flexibility in the model-specific unit employed, the type is provided. [common_unit_types] lists the standard unit types that are tied to various parameters in BlackBear.

!table id=common_unit_types
       caption=Standard model-specific types of units expected by various models
      style=width:50%;
| Unit type            |
|----------------------|
| unitless             |
| length               |
| time                 |
| mass                 |
| density              |
| temperature          |
| thermal conductivity |
| energy               |
| stress               |
| fracture toughness   |
| fracture energy      |

## Consistent Unit Systems

As noted previously, using a consistent set of units in the finite-element model is essential for obtaining accurate results. This is done by choosing a set of base units (for quantities such as mass, length, time and temperature), and deriving the units for other quantities from those. [common_consistent_unit_systems] provides a table of base and derived units relevant for thermo-mechanical simulations in five commonly-used consistent unit systems, based on that provided in [!cite](sierra_sm_56). The names for these unit systems are generally based on the names of their base units. These include international system (SI), centimeter-gram-second (CGS), inch-pound-second (IPS), foot-pound-second (FPS), and millimeter-tonne-second (MMTS). Note that in this table, lb refers to pounds as a force unit (also known as pound-force). Also, while K and $\degree$R are listed as temperature units, $\degree$C and $\degree$F can be used alternatively whenever relative temperatures are acceptable.

!table id=common_consistent_unit_systems
       caption=Commonly used consistent unit systems
      style=width:50%;
| Unit                        | SI | CGS | IPS | FPS | MMTS |
|-----------------------------|----|-----|-----|-----|------|
| mass                        | kg | g   | lb$\cdot$s$^2$/in | slug  | tonne   |
| length                      | m  | cm  | in  | ft  | mm   |
| time                        | s  | s   | s   | s   | s    |
| density                     | kg/m$^3$ | g/cm$^3$ | lb$\cdot$s$^2$/in$^4$ | slug/ft$^3$ | tonne/mm$^3$ |
| temperature                 | K | K | $\degree$R | $\degree$R | K |
| energy                      | J | erg | lb$\cdot$in | lb$\cdot$ft | mJ |
| force                       | N | dyne | lb | lb | N |
| stress, pressure, stiffness | Pa | dyne/cm$^2$ | psi | psf | MPa |
| fracture toughness          | Pa$\sqrt{\text{m}}$ |  (dyne/cm$^2$)$\sqrt{\text{cm}}$ | psi$\sqrt{\text{in}}$  | psi$\sqrt{\text{ft}}$ | MPa$\sqrt{\text{m}}$ |
| fracture energy             | J/m$^2$ | erg/cm$^2$ | lb/in | lb/ft | mJ/mm$^2$ |
| thermal conductivity        | W/(m$\cdot$K) | erg/(cm$\cdot$s$\cdot$K) | Btu/(in$\cdot$$\degree$R$\cdot$s) | Btu/(ft$\cdot$$\degree$R$\cdot$s) | W/(m$\cdot$K) |
