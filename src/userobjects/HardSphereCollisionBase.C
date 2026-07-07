//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic
//* confinement for Numerical Design, Engineering & Research,
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

#include "HardSphereCollisionBase.h"
#include "PICStudy.h"

InputParameters
HardSphereCollisionBase::validParams()
{
  auto params = CollisionBase::validParams();
  params.addClassDescription("Base class to handle elastic collisions");
  return params;
}

HardSphereCollisionBase::HardSphereCollisionBase(const InputParameters & parameters)
  : CollisionBase(parameters)
{
}

void
HardSphereCollisionBase::initialSetup()
{
  CollisionBase::initialSetup();
  _mass_a = _study->mass(_reactant_ids[0]);
  _mass_b = _study->mass(_reactant_ids[1]);
  _total_mass = _mass_a + _mass_b;
}

void
HardSphereCollisionBase::collideParticles(Ray & particle_a, Ray & particle_b) const
{
  _study->relativeVelocity(particle_a, particle_b, _relative_velocity);
  _study->centerOfMassVelocity(particle_a, particle_b, _center_of_mass_velocity);
  const Real relative_speed = _relative_velocity.norm();

  const Real cos_chi = 2.0 * _generator.rand() - 1.0;
  const Real sin_chi = std::sqrt(1.0 - cos_chi * cos_chi);
  const Real eps = 2.0 * M_PI * _generator.rand();

  _relative_velocity(0) = relative_speed * cos_chi;
  _relative_velocity(1) = relative_speed * sin_chi * std::cos(eps);
  _relative_velocity(2) = relative_speed * sin_chi * std::sin(eps);

  _study->setVelocity(particle_a,
                      _center_of_mass_velocity + _mass_b / _total_mass * _relative_velocity);
  _study->setVelocity(particle_b,
                      _center_of_mass_velocity - _mass_a / _total_mass * _relative_velocity);
}
