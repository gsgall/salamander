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
#include "ChangeRayMaxDistanceRayBC.h"
#include "ReflectParticleBC.h"

registerMooseObject("SalamanderApp", ChangeRayMaxDistanceRayBC);

InputParameters
ChangeRayMaxDistanceRayBC::validParams()
{
  auto params = ReflectParticleBC::validParams();
  params.addClassDescription(
      "Reflective boundary condition that resamples the particles velocity when it hits the "
      "boundary."
      "This condition requires that each component of velocity is sampled it's own distribution."
      "Currently this condition only works for x component.");
  params.addRequiredParam<Real>(
      "max_distance",
      "The seed for the random number generator used by the velocity sampler on the boundary.");

  return params;
}

ChangeRayMaxDistanceRayBC::ChangeRayMaxDistanceRayBC(const InputParameters & params)
  : ReflectParticleBC(params), _max_distance(getParam<Real>("max_distance"))
{
}

void
ChangeRayMaxDistanceRayBC::onBoundary(const unsigned int num_applying)
{
  ReflectParticleBC::onBoundary(num_applying);

  changeRayMaxDistance(_max_distance);
}
