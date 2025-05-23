# PeriodicPICStudy

!syntax description /UserObjects/PeriodicPICStudy

!alert warning
Periodic simulations are only supported for one-dimensional simulations.

!alert warning
When performing periodic simulations [KillRayBC.md] boundary conditions must be utilized.

!alert warning title=Particle Output
When performing periodic simulations the [PeriodicParticleDataVectorPostprocessor.md] is required to gather particle data properly.

## Overview

In the implementation of the PeriodicPICStudy particles that are periodic are killed by the [KillRayBC.md] and then restarted at the proper location within the domain on the next timestep. These particles are stored differently from particles which have not hit boundaries at the end of each time step and as a result certain operations require special objects. For example to proper save all of the particle data when performing a periodic simulation the [PeriodicParticleDataVectorPostprocessor.md] must be utilized rather than the [PeriodicParticleDataVectorPostprocessor.md].

## Example Input File Syntax


!listing test/tests/userobjects/periodic_study/1d_periodic.i

!syntax parameters /UserObjects/PeriodicPICStudy

!syntax inputs /UserObjects/PeriodicPICStudy

!syntax children /UserObjects/PeriodicPICStudy
