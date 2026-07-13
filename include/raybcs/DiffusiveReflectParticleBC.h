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
#include "RayBoundaryConditionBase.h"

class PICStudy;
class DiffusiveReflectParticleBC : public RayBoundaryConditionBase
{
public:
  DiffusiveReflectParticleBC(const InputParameters & params);

  static InputParameters validParams();

  virtual void onBoundary(const unsigned int num_applying) override;

protected:
  const unsigned int _mesh_dimension;
  /// the temperature of the wall
  const Real _temperature;
  const PICStudy & _pic_study;
  MooseRandom _generator;
  Point _temporary_velocity;
};
