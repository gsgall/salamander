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
#include "VelocitiesFromTemperatureFunctionVelocityInitializer.h"
#include "Function.h"
#include "Constants.h"
#include "NormalBase.h"

registerMooseObject("SalamanderApp", VelocitiesFromTemperatureFunctionVelocityInitializer);

InputParameters
VelocitiesFromTemperatureFunctionVelocityInitializer::validParams()
{
  auto params = VelocityInitializerBase::validParams();
  params.addClassDescription("Provides particles where the each component of the velocity is "
                             "sampled from a provided distribution");
  params.addRequiredParam<FunctionName>("function",
                                        "The functor that will provide the temperature function");
  params.addRequiredRangeCheckedParam<Real>("mass", "mass > 0", "The mass of the particle");
  params.addRequiredParam<unsigned int>("seed", "The seed for the random number generator.");
  return params;
}

VelocitiesFromTemperatureFunctionVelocityInitializer::
    VelocitiesFromTemperatureFunctionVelocityInitializer(const InputParameters & parameters)
  : VelocityInitializerBase(parameters),
    _function(getFunction("function")),
    _mass(getParam<Real>("mass")),
    _seed(getParam<unsigned int>("seed"))
{
  _generator.seed(_seed);
}

const std::vector<Point>
VelocitiesFromTemperatureFunctionVelocityInitializer::getParticleVelocities(
    const size_t num_samples, const unsigned int seed) const
{
  auto velocities = std::vector<Point>(num_samples);

  return velocities;
}

void
VelocitiesFromTemperatureFunctionVelocityInitializer::setParticleVelocities(
    std::vector<Salamander::ParticleData> & particle_data, const unsigned int additional_seed) const
{
}

const std::vector<Point>
VelocitiesFromTemperatureFunctionVelocityInitializer::getParticleVelocities(
    const std::vector<Point> & positions, const unsigned int additional_seed) const
{
  auto velocities = std::vector<Point>(positions.size());
  for (size_t i = 0; i < velocities.size(); ++i)
  {
    const auto temperature = _function.value(_t, positions[i]);
    const auto standard_deviation = std::sqrt(Salamander::constants::k_b * temperature / _mass);
    auto & vel = velocities[i];
    for (size_t j = 0; j < 3; ++j)
    {
      vel(j) = NormalBase::quantile(_generator.rand(), 0, standard_deviation);
    }
  }

  return velocities;
}
