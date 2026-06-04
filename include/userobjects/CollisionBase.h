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

#include "GeneralUserObject.h"
#include "Ray.h"

class CollisionalPICStudy;
class CollisionBase : public GeneralUserObject
{
public:
  CollisionBase(const InputParameters & parameters);

  static InputParameters validParams();
  /**
   * Unused methods
   */
  ///@{
  virtual void initialize() override final {}
  virtual void finalize() override final {}
  virtual void execute() override final {}
  ///@}
  virtual const Real sampleCrossSection(Ray & particle_a, Ray & particle_b) const = 0;
  virtual void collideParticles(Ray & particle_a, Ray & particle_b) const = 0;

  const std::vector<unsigned int> & reactantIds() const;
  const std::vector<unsigned int> & productIds() const;

  const Real relativeSpeed(const Ray & particle_a, const Ray & particle_b) const;

protected:
  const Point particleVelocity(const Ray & particle) const;
  const Real centerOfMassEnergy(const Ray & particle_a, const Ray & particle_b) const;

  const CollisionalPICStudy & _study;
  const std::array<RayDataIndex, 3> _velocity_indicies;
  const RayDataIndex _weight_index;
  const RayDataIndex _mass_index;
  const RayDataIndex _species_index;
  std::vector<unsigned int> _reactant_ids;
  std::vector<unsigned int> _product_ids;
};
