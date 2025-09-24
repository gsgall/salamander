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
#include "RankTwoTensor.h"
#include "VelocitiesFromDistributionsVelocityInitializer.h"
#include "Distribution.h"
#include "MooseRandom.h"
#include <algorithm>

registerMooseObject("SalamanderApp", VelocitiesFromDistributionsVelocityInitializer);

InputParameters
VelocitiesFromDistributionsVelocityInitializer::validParams()
{
  auto params = VelocityInitializerBase::validParams();
  params.addClassDescription(
      "Provides particles with initial velocities where the speed is sampled from the provided "
      "distribution and directions are isotropically distributed.");
  params.addRequiredParam<std::vector<DistributionName>>(
      "distributions", "The distribution from which the particle speed will be sampled.");
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
VelocitiesFromDistributionsVelocityInitializer::getParticleVelocities(
    const size_t num_samples) const
{
  auto velocities = std::vector<Point>(num_samples);

  MooseRandom generator;
  generator.seed(_seed);

  std::generate(velocities.begin(),
                velocities.end(),
                [this, &generator]()
                {
                  return Point(_distributions[0]->quantile(generator.rand()),
                               _distributions[1]->quantile(generator.rand()),
                               _distributions[2]->quantile(generator.rand()));
                });

  return velocities;
}
