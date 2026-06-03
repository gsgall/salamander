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

#include "PICStudyBase.h"
#include "ParticleColliderBase.h"

InputParameters
ParticleColliderBase::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addClassDescription("Base class for ParticleStepper. Provides the basic implementation"
                             "for dimensional dependent velocity updating."
                             "And the ability to sample vector fields for use in a particle step");
  params.addParam<unsigned int>(
      "seed", 0, "The seed value for the random number generator used for collisions");
  return params;
}

ParticleColliderBase::ParticleColliderBase(const InputParameters & parameters)
  : GeneralUserObject(parameters), _generator()
{
  _generator.seed(getParam<unsigned int>("seed"));
}

void
ParticleColliderBase::collectParticleData(const PICStudyBase & study)
{
  _species_names = study.speciesNames();
  _species_ids = study.speciesIds();
  _species_index = study.speciesIndex();
  _weight_index = study.weightIndex();

  setupInternalData();
}

unsigned int
ParticleColliderBase::pairingFunction(const unsigned int species_id_1,
                                      const unsigned int species_id_2) const
{
  const auto helper = [](const unsigned int n) -> unsigned int { return n * (n + 1) / 2; };
  return helper(std::max(species_id_1, species_id_2)) + std::min(species_id_1, species_id_2);
}
