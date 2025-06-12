//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic
// confinement for Numerical Design, Engineering & Research,
//* A multiphysics application for modeling plasma facing components *
// https://github.com/idaholab/salamander
//* https://mooseframework.inl.gov/salamander
//*
//* SALAMANDER is powered by the MOOSE Framework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
//* Copyright 2025, Battelle Energy Alliance, LLC
//* ALL RIGHTS RESERVED
//*

#include "ParticleVelocityVectorPostprocessor.h"
#include "PICStudyBase.h"

registerMooseObject("SalamanderApp", ParticleVelocityVectorPostprocessor);

InputParameters
ParticleVelocityVectorPostprocessor::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();
  params.addClassDescription(
      "Collects data which is stored in RayData on particles on a per timestep basis.");
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the Ray");
  return params;
}

ParticleVelocityVectorPostprocessor::ParticleVelocityVectorPostprocessor(
    const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _study(getUserObject<PICStudyBase>("study")),
    _ray_data_indices({_study.getRayDataIndex("v_x"),
                       _study.getRayDataIndex("v_y"),
                       _study.getRayDataIndex("v_z")}),
    _data_values({&declareVector("v_x"), &declareVector("v_y"), &declareVector("v_z")})
{
}

void
ParticleVelocityVectorPostprocessor::initialize()
{
  for (auto & data : _data_values)
    data->clear();
}

void
ParticleVelocityVectorPostprocessor::execute()
{

  const auto rays = _study.getBankedParticles();
  for (const auto & ray : rays)
  {
    for (const auto i : make_range(0, 3))
    {
      _data_values[i]->push_back(ray->data(_ray_data_indices[i]));
    }
  }
}

void
ParticleVelocityVectorPostprocessor::finalize()
{
  for (auto data : _data_values)
    comm().gather(0, *data);
}
