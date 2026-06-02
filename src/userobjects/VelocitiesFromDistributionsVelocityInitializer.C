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
