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
//* Copyright 2025, Battelle Energy Alliance, LLC and North Carolina State University
//* ALL RIGHTS RESERVED
//*

#pragma once

#include "RayBoundaryConditionBase.h"

class ParticleBCBase : public RayBoundaryConditionBase
{
public:
  ParticleBCBase(const InputParameters & params);

  static InputParameters validParams();

  virtual void onBoundary(const unsigned int num_applying) = 0;

  /**
   * Updates the paticles velocity and direction to ensure that it has a
   * direction that is consistent with the spatial dimension and it's velocity data
   * is updated properly
   */
  void changeParticleVelocity(const Point & velocity);

private:
  /// an intermediate point value to be used during the particle velocity update function
  Point _temporary_update_velocity;

protected:
  /// Stores all of the indicies for all velocity indicies
  const std::vector<RayDataIndex> _velocity_indicies;
  /// this is a scratch instance of point. It allows people to do some
  /// intermediate computations if needed
  Point _temporary_velocity;
  /// the spatial dimension of the mesh being used
  const unsigned int _mesh_dimension;
};
