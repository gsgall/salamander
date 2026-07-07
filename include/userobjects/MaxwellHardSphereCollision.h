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

#include "HardSphereCollisionBase.h"

class MaxwellHardSphereCollision : public HardSphereCollisionBase
{
public:
  MaxwellHardSphereCollision(const InputParameters & parameters);

  static InputParameters validParams();

  virtual const Real sigmaCr(const Ray & particle_a, const Ray & particle_b) const override;
  virtual const Real
  estimateSigmaCrMax(const std::vector<std::shared_ptr<Ray>> & particles) const override;

protected:
  const Real _sigma_ref;
};
