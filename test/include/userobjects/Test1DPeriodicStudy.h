//* This file is part of FENIX: Fusion ENergy Integrated multiphys-X,
//* A multiphysics application for modeling plasma facing components
//* https://github.com/idaholab/fenix
//* https://mooseframework.inl.gov/fenix
//*
//* FENIX is powered by the MOOSE Framework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
//* Copyright 2024, Battelle Energy Alliance, LLC
//* ALL RIGHTS RESERVED
//*

#pragma once

#include "ParticleInitializerBase.h"
#include "TestInitializedPICStudy.h"
/**
 * Test study of initializing rays with an initializer that will
 * give all of the data needed for rays and ensure that results are parallel consistent
 * assuming that element ids do not change as a function of the number processes being
 * used to solve this
 */
class Test1DPeriodicStudy : public TestInitializedPICStudy
{
public:
  Test1DPeriodicStudy(const InputParameters & parameters);

  static InputParameters validParams();

  const std::vector<InitialParticleData> & getPeriodicRays() const
  {
    return _periodic_particles;
  };
protected:
  // the left most point in the domain
  Real _x_min; 
  // the right most point in the domain
  Real _x_max; 
  Real _local_x_min;
  Real _local_x_max;
  // the total length of the domain
  Real _domain_length;
  // where we still store the data needed to restart periodic particles
  std::vector<InitialParticleData> _periodic_particles;
  virtual void reinitializeParticles() override; 
  virtual void postExecuteStudy() override;
};
