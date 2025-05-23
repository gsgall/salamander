//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic
//confinement for Numerical Design, Engineering & Research,
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
  InputParameters params = PeriodicParticleDataVectorPostprocessor::validParams();
  return params;
}

TestPeriodicParticleDataVectorPostprocessor::TestPeriodicParticleDataVectorPostprocessor(
    const InputParameters & parameters)
  : PeriodicParticleDataVectorPostprocessor(parameters)
{
}


void
TestPeriodicParticleDataVectorPostprocessor::finalize()
{
  // This will communicate the data between processors
  PeriodicParticleDataVectorPostprocessor::finalize();
  // Rays cannot be sorted by ids currently in a consistent manner.
  // This is because of the current implementation of the periodic boundaries.
  // If a ray hits a boundary and needs to be moved to the other side currently
  // it is removed from banked rays and then a new replicated ray is used and claimed.
  // this means the same computional particle can have a different id between time steps
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
