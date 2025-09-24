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
#include "SpeedFromDistributionVelocityInitializer.h"
#include "Distribution.h"
#include "MooseRandom.h"
#include <algorithm>

registerMooseObject("SalamanderApp", SpeedFromDistributionVelocityInitializer);

InputParameters
SpeedFromDistributionVelocityInitializer::validParams()
{
  auto params = VelocityInitializerBase::validParams();
  params.addClassDescription(
      "Provides particles with initial velocities where the speed is sampled from the provided "
      "distribution and directions are isotropically distributed.");
  params.addRequiredParam<DistributionName>(
      "distribution", "The distribution from which the particle speed will be sampled.");
  return params;
}

SpeedFromDistributionVelocityInitializer::SpeedFromDistributionVelocityInitializer(
    const InputParameters & parameters)
  : VelocityInitializerBase(parameters),
    _distribution_name(getParam<DistributionName>("distribution"))
{
}

void
SpeedFromDistributionVelocityInitializer::initialSetup()
{
  _speed_distribution = &getDistributionByName(_distribution_name);
}

const std::vector<Point>
SpeedFromDistributionVelocityInitializer::getParticleVelocities(const size_t num_samples) const
{
  auto velocities = std::vector<Point>(num_samples);

  MooseRandom generator;
  generator.seed(_seed);

  std::generate(velocities.begin(),
                velocities.end(),
                [this, &generator]()
                {
                  const Real cos_chi = 2 * generator.rand() - 1;
                  const Real sin_chi = std::sqrt(1 - cos_chi * cos_chi);
                  const Real eps = 2 * M_PI * generator.rand();
                  return _speed_distribution->quantile(generator.rand()) *
                         Point(cos_chi, sin_chi * std::cos(eps), sin_chi * std::sin(eps));
                });

  return velocities;
}
