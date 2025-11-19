# Units in BlackBear

## General Concepts

BlackBear is a multiphysics simulation code based on the finite-element method. Finite-element models are inherently flexible in terms of the system of units that they employ. Because of that, most of the models in the MOOSE framework and modules that BlackBear is built on are not tied to a specific unit system.

However, when defining a model, it is essential to intentionally choose a unit system, and to consistently use that across the set of physics models used in the simulation. Even though many physics models may be flexible regarding the unit system employed, their inputs and outputs---which can include input parameters, field variables, model dimensions, and residuals---are typically physical quantities that are tied to a certain type of unit. For example, finite element mesh coordinates must be defined in length units, and solution time must be in time units. 

In some cases, physics models are based on built-in data that is defined in a specific unit system. In those cases, models may specify that specific parameters or coupled variables are in a certain unit system, or they may alternatively provide the user options for using the model with multiple alternative unit systems. Many of the models in BlackBear are tied to specific units, and when a certain unit is required for a model, that is noted in the documentation for that model. Use of International System (SI) units is generally recommended with BlackBear, and most models are designed to operate with those units.

## Documentation

Whenever a parameter for a model is expected to be in a specific unit or type of unit, this is documented within the information on that model's parameters. In the documentation for each parameter, there is a list of important information about that variable, and the unit is included in that list.

When the model requires only that a type of unit is used, but allows flexibility in the model-specific unit employed, the type is provided. [common_unit_types] lists the unit types that are tied to various parameters in BlackBear.

!table id=common_unit_types
       caption=Common types of units expected for models and examples of commonly used units for those unit types
      style=width:50%;
| Unit type            | Commonly used units |
|----------------------|---------------------|
| unitless             | explicitly defined as having no unit |
| length               | m, mm, $\mu$m       |
| time                 | s, day              |
| mass                 | g, kg               |
| density              | kg/m$^3$            |
| temperature          | K, $\degree$C       |
| thermal conductivity | W/(m$\cdot$K)       |
| energy               | J, MPa*m$^3$        |
| stress               | Pa, MPa             |
| fracture toughness   | Pa$\sqrt{\text{m}}$, MPa$\sqrt{\text{m}}$ |
| fracture energy      | J/m$^2$             |

