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

#include "MooseRandom.h"
#include "ParticleBCBase.h"
#include "Ray.h"

class VelocityInitializerBase;
class DiffusiveReflectionBC : public ParticleBCBase
{
public:
  DiffusiveReflectionBC(const InputParameters & params);

  static InputParameters validParams();

  virtual void onBoundary(const unsigned int num_applying) override;

protected:
  /// the temperature of the wall
  const Real _temperature;
  const RayDataIndex _mass_index;
  MooseRandom _generator;
};
