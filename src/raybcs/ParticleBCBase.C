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

#include "ParticleBCBase.h"
#include "PICStudyBase.h"
#include "RayBoundaryConditionBase.h"

InputParameters
ParticleBCBase::validParams()
{
  auto params = RayBoundaryConditionBase::validParams();
  params.addClassDescription(
      "Reflective boundary condition for particles that ensures velocity data is consistent with "
      "its direction after reflection.");
  return params;
}

ParticleBCBase::ParticleBCBase(const InputParameters & params)
  : RayBoundaryConditionBase(params),
    _temporary_update_velocity(Point(0.0, 0.0, 0.0)),
    _velocity_indicies(getStudy<PICStudyBase>().getVelocityIndicies()),
    _mesh_dimension(_fe_problem.mesh().dimension())
{
}

void
ParticleBCBase::changeParticleVelocity(const Point & velocity)
{
  for (size_t i = 0; i < 3; ++i)
    currentRay()->data(_velocity_indicies[i]) = velocity(i);

  for (size_t i = 0; i < _mesh_dimension; ++i)
    _temporary_update_velocity(i) = velocity(i);

  changeRayDirection(_temporary_update_velocity);
}
