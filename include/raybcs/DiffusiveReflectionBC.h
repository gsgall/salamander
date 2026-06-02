//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic
// confinement for Numerical Design, Engineering & Research,
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
//* Copyright 2025, Battelle Energy Alliance, LLC and North Carolina State University
//* ALL RIGHTS RESERVED
//*

#pragma once

#include "ParticleBCBase.h"

class VelocityInitializerBase;
class DiffusiveReflectionBC : public ParticleBCBase
{
public:
  DiffusiveReflectionBC(const InputParameters & params);

  static InputParameters validParams();

  virtual void onBoundary(const unsigned int num_applying) override;

protected:
  /// the random number generator used for sampling distributions
  const unsigned int _seed;
  /// the distributions that will be used for set the initial particle velocities
  const VelocityInitializerBase & _velocity_initializer;
  /// the direction that is the normal the boundary
  const unsigned int _reflection_direction;
};
