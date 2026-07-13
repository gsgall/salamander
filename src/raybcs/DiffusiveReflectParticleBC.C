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
#include "DiffusiveReflectParticleBC.h"
#include "MooseUtils.h"
#include "PICStudy.h"
#include "Weibull.h"
#include "Constants.h"
#include "Maxwellian.h"
#include "RayBoundaryConditionBase.h"
#include <libmesh/fuzzy_equals.h>

registerMooseObject("SalamanderApp", DiffusiveReflectParticleBC);

InputParameters
DiffusiveReflectParticleBC::validParams()
{
  auto params = RayBoundaryConditionBase::validParams();
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

DiffusiveReflectParticleBC::DiffusiveReflectParticleBC(const InputParameters & params)
  : RayBoundaryConditionBase(params),
    _mesh_dimension(_fe_problem.mesh().dimension()),
    _temperature(getParam<Real>("temperature")),
    _pic_study(getStudy<PICStudy>())
{
  _generator.seed(getParam<unsigned int>("seed"));
}

void
DiffusiveReflectParticleBC::onBoundary(const unsigned int num_applying)
{
  const auto & normal = _study.getSideNormal(_current_elem, _current_intersected_side, _tid);
  const auto & particle = currentRay();

  // const Real normal_velocity
  const auto mass = _pic_study.mass(*particle);
  const Real shape = 2;
  const Real scale = std::sqrt(2.0 * Salamander::constants::k_b / mass * _temperature);

  _temporary_velocity(0) = Weibull::quantile(_generator.rand(), 0, scale, shape);

  if (_temporary_velocity(0) * normal(0) > 0)
    _temporary_velocity(0) *= -1.0;
  // setting the tangential velocities and this does assume that the normal is always the x
  // component
  _temporary_velocity(1) = Maxwellian::quantile(_generator.rand(), mass, _temperature);

  _temporary_velocity(2) = Maxwellian::quantile(_generator.rand(), mass, _temperature);

  _pic_study.setVelocity(*particle, _temporary_velocity);

  const Real time_left = _dt * (1 - particle->distance() / particle->maxDistance());
  Real directional_speed = 0;

  for (size_t i = 0; i < _mesh_dimension; ++i)
  {
    directional_speed += _temporary_velocity(i) * _temporary_velocity(i);
  }
  directional_speed = std::sqrt(directional_speed);

  for (size_t i = _mesh_dimension; i < 3; ++i)
  {
    _temporary_velocity(i) = 0.0;
  }

  changeRayDirection(_temporary_velocity);
  changeRayMaxDistance(directional_speed * time_left);
}
