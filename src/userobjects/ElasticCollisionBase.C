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

#include "ElasticCollisionBase.h"
#include "CollisionalPICStudy.h"

InputParameters
ElasticCollisionBase::validParams()
{
  auto params = CollisionBase::validParams();
  params.addClassDescription("Base class to handle elastic collisions");
  return params;
}

ElasticCollisionBase::ElasticCollisionBase(const InputParameters & parameters)
  : CollisionBase(parameters)
{
}

void
ElasticCollisionBase::collideParticles(Ray & particle_a, Ray & particle_b) const
{
  auto vel_a = particleVelocity(particle_a);
  const auto m_a = particle_a.data(_mass_index);

  auto vel_b = particleVelocity(particle_b);
  const auto m_b = particle_b.data(_mass_index);
  const auto total_mass = m_a + m_b;

  Point relative_velocity = vel_a - vel_b;
  const Real relative_speed = relative_velocity.norm();
  const Point center_of_mass_velocity = centerOfMassVelocity(particle_a, particle_b);

  const Real cos_chi = 2 * _generator.rand();
  const Real sin_chi = std::sqrt(1 - cos_chi * cos_chi);
  const Real eps = 2 * M_PI * _generator.rand();

  relative_velocity(0) = relative_speed * cos_chi;
  relative_velocity(1) = relative_speed * sin_chi * std::cos(eps);
  relative_velocity(2) = relative_speed * sin_chi * std::sin(eps);

  vel_a = center_of_mass_velocity + m_b / (total_mass)*relative_velocity;
  vel_b = center_of_mass_velocity + m_a / (total_mass)*relative_velocity;

  _study->setVelocityData(particle_a, vel_a);
  _study->setVelocityData(particle_b, vel_b);
}
