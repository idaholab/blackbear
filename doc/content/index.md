!config renderer breadcrumbs=False scrollspy=False

# BlackBear class=center style=font-weight:600;font-size:350%;

### Structural Material Degradation Simulation Code class=center style=font-weight:200;font-size:200%


BlackBear is an application for modeling degradation phenomena in materials such as concrete and
steel used in civil structures, as well as the response of those structures to the loading conditions
that they are expected to safely withstand.  It is based on the finite element method and models
interactions between the coupled physical systems involved.  BlackBear is based on the MOOSE
framework, and builds on the MOOSE framework and modules for many of its core capabilities.

!row!
!col! class=s12 m4 l4 icon=get_app
### [Getting Started](getting_started/RunningBlackBear.md) class=center style=font-weight:200;

Quickly learn how to obtain the BlackBear source code, compile an executable, and run simulations
with these instructions.
!col-end!

!col! class=s12 m4 l4 icon=settings
### [Code Reference](systems/index.md) class=center style=font-weight:200;

BlackBear provides capabilities that can be applied to a wide variety of problems. The Code Reference
provides detailed documentation of specific code features.
!col-end!

!col! class=s12 m4 l4 icon=assessment
### [Demonstration](demonstration/Placeholder.md) class=center style=font-weight:200;

There are a number of example problems to demonstrate usage of BlackBear. Details of those models are
provided here.
!col-end!
!row-end!

### Blackbear is built on MOOSE style=clear:both;

BlackBear is based on [MOOSE](http://mooseframework.org). It is an extremely flexible environment that
permits the solution of coupled physics problems of varying size and dimensionality. These can be
solved using computer hardware appropriate for the model size, ranging from laptops and workstations
to large high performance computers.

!media media/inl_blue.png style=float:right;width:30%;margin-left:30px;

Code reliability is a central principle in code development, and this project employs a well defined
development and testing strategy.  Code changes are only merged into the repository after both a
manual code review and the automated regression test system have been completed.  The testing process
and status of BlackBear is available at [www.moosebuild.inl.gov](https://moosebuild.inl.gov/repo/5/).

BlackBear and MOOSE are developed by the Idaho National Laboratory by a team of computer scientists
and engineers and is supported by various funding agencies including the
[United States Department of Energy](http://energy.gov).  Development of these codes is ongoing at
INL and by collaborators throughout the world.
