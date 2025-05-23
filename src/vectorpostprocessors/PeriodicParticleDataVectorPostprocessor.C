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
//* Copyright 2025, Battelle Energy Alliance, LLC
//* ALL RIGHTS RESERVED
//*

#include "PeriodicPICStudy.h"
#include "PICStudyBase.h"
#include "PeriodicParticleDataVectorPostprocessor.h"

registerMooseObject("SalamanderApp", PeriodicParticleDataVectorPostprocessor);

InputParameters
PeriodicParticleDataVectorPostprocessor::validParams()
{
  InputParameters params = ParticleDataVectorPostprocessor::validParams();
  return params;
}

PeriodicParticleDataVectorPostprocessor::PeriodicParticleDataVectorPostprocessor(
    const InputParameters & parameters)
  : ParticleDataVectorPostprocessor(parameters),
    _periodic_study(getUserObject<PeriodicPICStudy>("study")),
    _additional_output_count(
        getParam<std::vector<std::string>>("additional_ray_data_outputs").size())
{
}

void
PeriodicParticleDataVectorPostprocessor::execute()
{
  // we will collect all the data from the particles that are not periodic
  // first and then add the data from the particles which are periodic
  ParticleDataVectorPostprocessor::execute();

  const auto & periodic_particles = _periodic_study.getPeriodicParticles();

  for (const auto & data : periodic_particles)
  {
    // storing the time at which the particle position is known
    _data_values[0]->push_back(_t);
    // storing the time at which the particle velocity is known
    _data_values[1]->push_back(_t - _dt / 2);
    for (const auto i : make_range(2, 5))
      _data_values[i]->push_back(data.position(i - 2));
    // storing all of the components of velocity
    for (const auto i : make_range(5, 8))
      _data_values[i]->push_back(data.velocity(i - 5));

    for (const auto i : make_range(_additional_output_count))
    {

      const auto & name = _study.getRayDataName(_ray_data_indices[3 + i]);

      if (name == "charge")
      {
        _data_values[8 + i]->push_back(data.charge);
      }
      else if (name == "mass")
      {
        _data_values[8 + i]->push_back(data.mass);
      }
      else if (name == "weight")
      {
        _data_values[8 + i]->push_back(data.weight);
      }
    }
  }
}

void
PeriodicParticleDataVectorPostprocessor::finalize()
{
  // This will communicate the data between processors
  ParticleDataVectorPostprocessor::finalize();
}
