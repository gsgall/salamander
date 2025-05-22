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

#include "TestPlacedParticleInitializer.h"

registerMooseObject("SalamanderTestApp", TestPlacedParticleInitializer);

InputParameters
TestPlacedParticleInitializer::validParams()
{
  auto params = ParticleInitializerBase::validParams();
  params.addRequiredParam<std::vector<Point>>("start_points",
                                              "The point(s) where the ray(s) start");
  params.addRequiredParam<std::vector<Point>>(
      "start_velocities",
      "The direction(s) that the ray(s) start in (does not need to be normalized)");
  params.addParam<Real>(
      "weight", 0, "The number of physical particles a computational particle represents");
  // these are unused in this class but the base class checks the size of this to make sure it is 3
  params.setParameters<std::vector<DistributionName>>("velocity_distributions",
                                                      std::vector<DistributionName>{"a", "a", "a"});
  return params;
}

TestPlacedParticleInitializer::TestPlacedParticleInitializer(const InputParameters & parameters)
  : ParticleInitializerBase(parameters),
    _start_points(getParam<std::vector<Point>>("start_points")),
    _start_velocities(getParam<std::vector<Point>>("start_velocities")),
    _mass(getParam<Real>("mass")),
    _charge(getParam<Real>("charge")),
    _weight(getParam<Real>("weight"))
{
  if (_start_points.size() != _start_velocities.size())
    paramError("start_velocities", "Must be the same size as 'start_points'");
}

std::vector<InitialParticleData>
TestPlacedParticleInitializer::getParticleData() const
{
  std::vector<InitialParticleData> particle_data;

  for (unsigned int i = 0; i < _start_points.size(); ++i)
  {
    const Elem * particle_elem = nullptr; 
    // we'll check to see if this processor owns any of the points
    // where we want to put particles. We coudl do this with replicated rays
    // however we don't want to have to use replicated rays everytime we need 
    // a new ray during the transient study
    for (auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
    {
      if (elem->contains_point(_start_points[i]))
      {
        particle_elem = elem;
      }
    }
    // the pointer will be null in the case that the processor doesn't own the point
    if (particle_elem == nullptr)
      continue;
    auto & data = particle_data.emplace_back(); 

    data.position = _start_points[i];
    data.velocity = _start_velocities[i];
    data.mass = _mass;
    data.charge = _charge;
    data.weight = _weight;
    data.species = "";
    data.elem = particle_elem;
  }

  return particle_data;
}
