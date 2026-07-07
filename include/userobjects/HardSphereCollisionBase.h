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

#include "CollisionBase.h"

class HardSphereCollisionBase : public CollisionBase
{
public:
  HardSphereCollisionBase(const InputParameters & parameters);

  static InputParameters validParams();

  virtual void initialSetup() override;
  virtual void collideParticles(Ray & particle_a, Ray & particle_b) const override;
protected:
  Real _mass_a;
  Real _mass_b;
  Real _total_mass;

  mutable Point _relative_velocity;
  mutable Point _center_of_mass_velocity;
};
