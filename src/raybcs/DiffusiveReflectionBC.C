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
#include "DiffusiveReflectionBC.h"
#include "MooseUtils.h"
#include "libmesh/fuzzy_equals.h"
#include "PICStudyBase.h"
#include "Constants.h"

registerMooseObject("SalamanderApp", DiffusiveReflectionBC);

InputParameters
DiffusiveReflectionBC::validParams()
{
  auto params = ParticleBCBase::validParams();
  params.addClassDescription(
      "Reflective boundary condition that resamples the particles velocity when it hits the "
      "boundary."
      "This condition requires that each component of velocity is sampled it's own distribution."
      "Currently this condition only works for x component.");
  params.addParam<unsigned int>(
      "seed",
      0,
      "The seed for the random number generator used by the velocity sampler on the boundary.");

  params.addRequiredRangeCheckedParam<Real>(
      "temperature", "temperature > 0", "The temperature of the wall.");
  return params;
}

DiffusiveReflectionBC::DiffusiveReflectionBC(const InputParameters & params)
  : ParticleBCBase(params),
    _temperature(getParam<Real>("temperature")),
    _mass_index(getStudy<PICStudyBase>().massIndex())
{
  _generator.seed(getParam<unsigned int>("seed"));
}

void
DiffusiveReflectionBC::onBoundary(const unsigned int num_applying)
{
  // The direction this Ray reflects off this boundary
  const auto & normal = _study.getSideNormal(_current_elem, _current_intersected_side, _tid);

  const Real most_probable_speed =
      std::sqrt(2.0 * Salamander::constants::k_b * _temperature / currentRay()->data(_mass_index));
  Real perpendicular_speed;
  do
  {
    perpendicular_speed = most_probable_speed * std::sqrt(-std::log(_generator.rand()));
  } while (MooseUtils::absoluteFuzzyEqual(perpendicular_speed, 0));

  const Real tangential_speed = most_probable_speed * std::sqrt(-std::log(_generator.rand()));
  const Real theta = 2 * M_PI * _generator.rand();

  _temporary_velocity(0) = perpendicular_speed * -normal(0);
  _temporary_velocity(1) = tangential_speed * std::sin(theta);
  _temporary_velocity(2) = tangential_speed * std::cos(theta);
  _new_velocities.push_back(_temporary_velocity);

  changeParticleVelocity(_temporary_velocity);
}
