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
//* Copyright 2025, Battelle Energy Alliance, LLC
//* ALL RIGHTS RESERVED
//*
#include "VelocitiesFromDistributionsVelocityInitializer.h"
#include "Distribution.h"
#include "MooseRandom.h"
#include <algorithm>

registerMooseObject("SalamanderApp", VelocitiesFromDistributionsVelocityInitializer);

InputParameters
VelocitiesFromDistributionsVelocityInitializer::validParams()
{
  auto params = VelocityInitializerBase::validParams();
  params.addClassDescription("Provides particles where the each component of the velocity is "
                             "sampled from a provided distribution");
  params.addRequiredParam<std::vector<DistributionName>>(
      "distributions", "The distribution from which the particle velocities will be sampled.");
  return params;
}

VelocitiesFromDistributionsVelocityInitializer::VelocitiesFromDistributionsVelocityInitializer(
    const InputParameters & parameters)
  : VelocityInitializerBase(parameters),
    _distribution_names(getParam<std::vector<DistributionName>>("distributions"))
{
}

void
VelocitiesFromDistributionsVelocityInitializer::initialSetup()
{
  for (const auto & name : _distribution_names)
  {
    _distributions.push_back(&getDistributionByName(name));
  }
}

const std::vector<Point>
VelocitiesFromDistributionsVelocityInitializer::getParticleVelocities(const size_t num_samples,
                                                                      const unsigned int seed) const
{
  auto velocities = std::vector<Point>(num_samples);

  _generator.seed(seed + _seed);

  std::generate(velocities.begin(),
                velocities.end(),
                [this]()
                {
                  return Point(_distributions[0]->quantile(_generator.rand()),
                               _distributions[1]->quantile(_generator.rand()),
                               _distributions[2]->quantile(_generator.rand()));
                });

  return velocities;
}

void
VelocitiesFromDistributionsVelocityInitializer::setParticleVelocities(
    std::vector<Salamander::ParticleData> & particle_data, const unsigned int additional_seed) const
{
  _generator.seed(additional_seed + _seed);
  for (auto & data : particle_data)
  {
    for (size_t i = 0; i < 3; ++i)
    {
      data.velocity(i) = _distributions[i]->quantile(_generator.rand());
    }
  }
}

const std::vector<Point>
VelocitiesFromDistributionsVelocityInitializer::getParticleVelocities(
    const std::vector<Point> & positions, const unsigned int additional_seed) const
{
  _generator.seed(_seed + additional_seed);

  auto velocities = std::vector<Point>(positions.size());
  for (size_t i = 0; i < velocities.size(); ++i)
  {
    auto & vel = velocities[i];
    for (size_t j = 0; j < 3; ++j)
    {
      vel(j) = _distributions[j]->quantile(_generator.rand());
    }
  }
  return velocities;
}
