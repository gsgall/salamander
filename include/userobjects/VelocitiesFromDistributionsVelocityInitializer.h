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

#pragma once

#include "MooseRandom.h"
#include "VelocityInitializerBase.h"

class Distribution;
class VelocitiesFromDistributionsVelocityInitializer : public VelocityInitializerBase
{
public:
  VelocitiesFromDistributionsVelocityInitializer(const InputParameters & parameters);

  static InputParameters validParams();

  virtual const std::vector<Point> getParticleVelocities(size_t num_samples,
                                                         const unsigned int seed) const override;

  virtual const Point getParticleVelocity() const override;
  /**
   * We use this to get the actual distribution object that we need to use in order to sample the
   * particle speeds
   */
  virtual void initialSetup() override;

protected:
  /// the random number generator object that will be used to sample distributions
  MooseRandom _generator;
  /// the distributions that will be used for set the initial particle velocities
  std::vector<Distribution const *> _distributions;
  /// Velocity distribution names
  const std::vector<DistributionName> & _distribution_names;
};
