# VelocitiesFromDistributionsVelocityInitializer

!syntax description /UserObjects/VelocitiesFromDistributionsVelocityInitializer

## Overview

This initializer will sample each component of a particles velocity independently from the list of three provided distributions.
One of the main use cases for this object may to initialize particles at a specific temperature, sampling velocities from Maxwellian distributions.

## Example Input File Syntax

!listing test/tests/userobjects/velocity_initialization/maxwellian.i block=UserObjects/velocity_initializer

!syntax parameters /UserObjects/VelocitiesFromDistributionsVelocityInitializer

!syntax inputs /UserObjects/VelocitiesFromDistributionsVelocityInitializer

!syntax children /UserObjects/VelocitiesFromDistributionsVelocityInitializer
