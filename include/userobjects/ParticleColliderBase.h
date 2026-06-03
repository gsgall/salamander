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

class Ray;
class PICStudyBase;
class ParticleColliderBase : public GeneralUserObject
{
public:
  ParticleColliderBase(const InputParameters & parameters);

  static InputParameters validParams();
  /**
   * Unused methods
   */
  ///@{
  virtual void initialize() override final {}
  virtual void finalize() override final {}
  virtual void execute() override final {}
  ///@}

  virtual void collideParticles(const std::vector<std::shared_ptr<Ray>> & particles) = 0;

  void collectParticleData(const PICStudyBase & study);

protected:
  MooseRandom _generator;
  RayDataIndex _species_index;
  RayDataIndex _weight_index;
  std::vector<std::string> _species_names;
  std::vector<unsigned int> _species_ids;
  std::vector<Real> _random_numbers;

  virtual void setupInternalData() = 0;
  unsigned int pairingFunction(const unsigned int species_id_1,
                               const unsigned int species_id_2) const;
};
