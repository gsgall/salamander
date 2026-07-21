//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic
// confinement for Numerical Design, Engineering & Research,
//* A multiphysics application for modeling plasma facing components
//* https://github.com/idaholab/salamander
//* https://mooseframework.inl.gov/salamander
//*
//* SALAMANDER is powered by the MOOSE Framework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
//* Copyright 2025, Battelle Energy Alliance, LLC and North Carolina State University
//* ALL RIGHTS RESERVED
//*
#include "ChangeRayVelocity.h"
#include "ReflectParticleBC.h"

registerMooseObject("SalamanderApp", ChangeRayVelocity);

InputParameters
ChangeRayVelocity::validParams()
{
  auto params = ReflectParticleBC::validParams();
  params.addClassDescription(
      "Reflective boundary condition that resamples the particles velocity when it hits the "
      "boundary."
      "This condition requires that each component of velocity is sampled it's own distribution."
      "Currently this condition only works for x component.");
  params.addRequiredParam<Point>(
      "velocity", "The new velocity particles will have once they leave the boundary");
  return params;
}

ChangeRayVelocity::ChangeRayVelocity(const InputParameters & params)
  : ReflectParticleBC(params),
    _velocity(getParam<Point>("velocity")),
    _directional_speed(
        [&]()
        {
          Real temporary_speed = 0;
          for (size_t i = 0; i < _mesh_dimension; ++i)
          {
            temporary_speed += _velocity(i) * _velocity(i);
          }
          return temporary_speed;
        }())

{
}

void
ChangeRayVelocity::onBoundary(const unsigned int num_applying)
{
  const auto & particle = currentRay();

  const Real time_left = _dt * (1 - particle->distance() / particle->maxDistance());

  changeRayDirection(_velocity);
  changeRayMaxDistance(_directional_speed * time_left);
}
