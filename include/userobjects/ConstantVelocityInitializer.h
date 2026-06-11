//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic
//* confinement for Numerical Design, Engineering & Research,
//* A multiphysics application for modeling plasma facing components *
// https://github.com/idaholab/salamander
//* https://mooseframework.inl.gov/salamander
//*
//* SALAMANDER is powered by the MOOSE Framework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
//* Copyright 2025, Battelle Energy Alliance, LLC and North Carolina State University
//* ALL RIGHTS RESERVED
//*

#pragma once

#include "VelocityInitializerBase.h"

class ConstantVelocityInitializer : public VelocityInitializerBase
{
public:
  ConstantVelocityInitializer(const InputParameters & parameters);

  static InputParameters validParams();

  virtual const std::vector<Point>
  getParticleVelocities(const size_t num_samples,
                        const unsigned int additional_seed) const override;

  virtual void setParticleVelocities(std::vector<Salamander::ParticleData> & particle_data,
                                     const unsigned int additional_seed) const override;
  virtual const std::vector<Point>
  getParticleVelocities(const std::vector<Point> & positions,
                        const unsigned int additional_seed) const override;

protected:
  const std::vector<Point> _velocities;
};
