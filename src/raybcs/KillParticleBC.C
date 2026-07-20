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

#include "KillParticleBC.h"
#include "PICStudy.h"
#include <libmesh/int_range.h>
registerMooseObject("SalamanderApp", KillParticleBC);

InputParameters
KillParticleBC::validParams()
{
  auto params = ReflectRayBC::validParams();
  params.addClassDescription("Kills a particle and zeros the velocity data in the particle");
  return params;
}

KillParticleBC::KillParticleBC(const InputParameters & params)
  : ReflectRayBC(params), _pic_study(getStudy<PICStudy>()), _zero_velocity(0.0, 0.0, 0.0)
{
}

void
KillParticleBC::onBoundary(const unsigned int num_applying)
{
  auto & particle = currentRay();
  _pic_study.setVelocity(*particle, _zero_velocity);
  particle->setShouldContinue(false);
}
