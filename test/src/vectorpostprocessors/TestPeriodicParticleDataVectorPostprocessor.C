//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic confinement for Numerical Design, Engineering & Research,
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
#include "TestPeriodicParticleDataVectorPostprocessor.h"

registerMooseObject("SalamanderTestApp", TestPeriodicParticleDataVectorPostprocessor);

InputParameters
TestPeriodicParticleDataVectorPostprocessor::validParams()
{
  InputParameters params = ParticleDataVectorPostprocessor::validParams();
  return params;
}

TestPeriodicParticleDataVectorPostprocessor::TestPeriodicParticleDataVectorPostprocessor(
    const InputParameters & parameters)
  : ParticleDataVectorPostprocessor(parameters), 
  _periodic_study(getUserObject<PeriodicPICStudy>("study")),
  _additional_output_count(getParam<std::vector<std::string>>("additional_ray_data_outputs").size())
{
}


void
TestPeriodicParticleDataVectorPostprocessor::execute()
{
  // This does end up looping over the rays twice,
  // but since it will only be for testing, this is fine.
  // It helps to test the behaviour of the object people will actually
  // use without requiring it to store meta data that is not important for them.
  ParticleDataVectorPostprocessor::execute();

  const auto & periodic_rays = _periodic_study.getPeriodicRays(); 

  for (const auto & data : periodic_rays)
  {
    // storing the time at which the particle position is known
    _data_values[0]->push_back(_t);
    // storing the time at which the particle velocity is known
    _data_values[1]->push_back(_t - _dt / 2);
    for (const auto i : make_range(2, 5))
      _data_values[i]->push_back(data.position(i - 2));
    
    for (const auto i : make_range(5, 8))
      _data_values[i]->push_back(data.velocity(i - 5));

    for (const auto i : make_range(_additional_output_count))
    {

      const auto & name = _study.getRayDataName(_ray_data_indices[3 + i]);
      
      if (name == "charge")
      {
        _data_values[8 + i]->push_back(data.charge);
      } else if (name == "mass")
      {
        _data_values[8 + i]->push_back(data.mass);
      } else if (name == "weight")
      {
        _data_values[8 + i]->push_back(data.weight);
      }
    }
  }
}

void
TestPeriodicParticleDataVectorPostprocessor::finalize()
{
  // This will communicate the data between processors
  ParticleDataVectorPostprocessor::finalize();
  // Rays cannot be sorted by ids currently in a consistent manner. 
  // This is because of the current implementation of the periodic boundaries. 
  // If a ray hits a boundary and needs to be moved to the other side currently 
  // it is removed from banked rays and then a new replicated ray is used and claimed. 
  // this means the same computional particle can have a different id between time stepsear
  std::vector<size_t> indicies;
  indicies.resize(_data_values.front()->size());
  std::iota(indicies.begin(), indicies.end(), 0);
  std::sort(indicies.begin(),
            indicies.end(),
            [&](size_t a, size_t b) -> bool
            { return (*_data_values[2])[a] < (*_data_values[2])[b]; });

  for (auto data : _data_values)
    Moose::applyIndices(*data, indicies);
}
