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
#include "VelocityInitializerBase.h"
#include "libmesh/fuzzy_equals.h"

registerMooseObject("SalamanderApp", DiffusiveReflectionBC);

InputParameters
DiffusiveReflectionBC::validParams()
{
  auto params = ParticleBCBase::validParams();
  params.addClassDescription(
      "Reflective boundary condition that resamples the particles velocity when it hits the "
      "boundary."
      "This condition requires that each component of velocity is sampled it's own distribution.");
  params.addRangeCheckedParam<unsigned int>("reflection_direction",
                                            "reflection_direction < 3",
                                            "The direction which the rays are being reflects");
  params.addParam<unsigned int>(
      "seed",
      0,
      "The seed for the random number generator used by the velocity sampler on the boundary.");

  params.addRequiredParam<UserObjectName>(
      "velocity_initializer",
      "The user object that will generate the initial velocities for all of the particles.");
  return params;
}

DiffusiveReflectionBC::DiffusiveReflectionBC(const InputParameters & params)
  : ParticleBCBase(params),
    _seed(getParam<unsigned int>("seed")),
    _velocity_initializer(getUserObject<VelocityInitializerBase>("velocity_initializer")),
    _reflection_direction(getParam<unsigned int>("reflection_direction"))
{
}

void
DiffusiveReflectionBC::onBoundary(const unsigned int num_applying)
{
  const auto & normal = _study.getSideNormal(_current_elem, _current_intersected_side, _tid);
  for (const auto i : make_range(3))
  {
    mooseAssert(libMesh::absolute_fuzzy_equals(normal(i), 0) ||
                    libMesh::absolute_fuzzy_equals(std::abs(normal(i)), 1.0),
                "This boundary condition only properly supports hypercube shaped domains.");
  }

  _temporary_velocity = _velocity_initializer.getParticleVelocities(1, _seed)[0];

  if (std::signbit(-_temporary_velocity(_reflection_direction) * normal(_reflection_direction)))
  {
    _temporary_velocity(_reflection_direction) = -_temporary_velocity(_reflection_direction);
  }

  changeParticleVelocity(_temporary_velocity);
}
