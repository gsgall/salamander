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
#include "MooseRandom.h"
#include "Ray.h"

class CollisionBase;
class CollisionalPICStudy;

class ParticleColliderBase : public GeneralUserObject
{
public:
  ParticleColliderBase(const InputParameters & parameters);

  static InputParameters validParams();
  virtual void initialSetup() override;

  /**
   * Unused methods
   */
  ///@{
  virtual void initialize() override final {}
  virtual void finalize() override final {}
  virtual void execute() override final {}
  ///@}

  virtual void initializeInternalData(const std::vector<std::shared_ptr<Ray>> & particles) = 0;
  virtual void collideParticles(const std::vector<std::shared_ptr<Ray>> & particles) = 0;

protected:
  MooseRandom _generator;
  const CollisionalPICStudy * _study;
  std::array<unsigned int, 3> _velocity_indicies;
  RayDataIndex _species_index;
  RayDataIndex _weight_index;
  RayDataIndex _mass_index;
  std::vector<unsigned int> _species_ids;
  std::vector<std::vector<size_t>> _particle_indicies;

  std::vector<std::vector<const CollisionBase *>> _collision_objects;
  std::vector<std::vector<Real>> _temporary_xsecs;

  unsigned int pairingFunction(const unsigned int species_id_1,
                               const unsigned int species_id_2) const;
};
