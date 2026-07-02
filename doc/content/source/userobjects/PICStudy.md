# PICStudy


All particle-in-cell (PIC) simulations are run through the PICStudy class.
This object is a specialiation of the [RayTracingStudy.md].
Particles are represented by `Ray` objects and the specialization of the study declares a few pieces of additional `RayData` needed to perform PIC simulations.
The following pieces of additional data are declared in this study.

!table caption=Registered RayData
| Description | Registered Name | Index Member Variable |
| - | - | - |
| X Velocity Component | `"v_x"` | `_v_x_index` |
| Y Velocity Component | `"v_y"` | `_v_y_index` |
| Z Velocity Component | `"v_z"` | `_v_z_index` |
| Particle Weight | `"weight"` | `_weight_index`
| Particle Mass | `"mass"` | `_mass_index`
| Particle Charge | `"charge"` | `_charge_index`

However, developers who are adding capabilities to SALAMANDER should access the particle properties directly via the getter methods of the `PICStudy` class.
Additionally, after particles have finished tracing for a single time step they may be accessed via the `particles` method.

!alert note title=Dimensionality of Simulations
When performing a one dimensional PIC simulation the mesh dimension must be `x`.
Similarly for two dimensional PIC simulations the two dimensions must be `x` and `y`.

# Setting Initial Conditions

Unlike for other MOOSE based simulations, initial conditions for particles cannot be setup using the `IC` system.
Instead the `PICStudy` accepts a list of [ParticleInitializerBase.md]'s these objects provide the data required for each particle to the `PICStudy` and then the study itself will take care of creating the actual particles.
Additionally, each [ParticleInitializerBase.md] object also accepts a [VelocityInitializerBase.md] instance.
Location, and velocity selection are seperated in order to enable the use of different initial velocity conditions for any given spatial initial condition.

# Particle Collisions

!alert construction title=Collisions
Currently collision capabilities in SALAMANDER are under development and are not currently available for use.

## Example Input Syntax

!listing test/tests/userobjects/velocity_initialization/maxwellian.i block=UserObjects

!syntax parameters /UserObjects/PICStudy

!syntax inputs /UserObjects/PICStudy

!syntax children /UserObjects/PICStudy

