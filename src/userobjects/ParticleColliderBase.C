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

#include "ParticleColliderBase.h"
#include "CollisionalPICStudy.h"
#include "CollisionBase.h"

InputParameters
ParticleColliderBase::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addClassDescription("Base class for ParticleStepper. Provides the basic implementation"
                             "for dimensional dependent velocity updating."
                             "And the ability to sample vector fields for use in a particle step");
  params.addRequiredParam<UserObjectName>("study", "The study for the system.");
  params.addRequiredParam<std::vector<UserObjectName>>(
      "collision_objects",
      "The objects that contain the actual logic to carry out the collisions required.");
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
ParticleColliderBase::initialSetup()
{
  _study = &getUserObject<CollisionalPICStudy>("study");
  _species_index = _study->speciesIndex();
  _weight_index = _study->weightIndex();
  _mass_index = _study->massIndex();
  _species_ids = _study->speciesIds();
  _particle_indicies.resize(_species_ids.size());
  const auto & names = getParam<std::vector<UserObjectName>>("collision_objects");
  const auto total_pairs = pairingFunction(_species_ids.size(), _species_ids.size()) - 1;

  _collision_objects.resize(total_pairs);
  _temporary_xsecs.resize(total_pairs);

  for (const auto & name : names)
  {
    const auto & collision = getUserObjectByName<CollisionBase>(name);
    const auto reactant_ids = collision.reactantIds();
    const auto pair_id = pairingFunction(reactant_ids[0], reactant_ids[1]);
    _collision_objects[pair_id].push_back(&collision);
  }

  for (size_t i = 0; i < total_pairs; ++i)
  {
    _temporary_xsecs[i].resize(_collision_objects[i].size());
  }

  const auto & elem_range = *_fe_problem.mesh().getActiveLocalElementRange();
  const auto num_elems = std::distance(elem_range.begin(), elem_range.end());

  _reaction_rates.resize(num_elems);

  for (auto & elem_rates : _reaction_rates)
  {
    elem_rates.resize(total_pairs);
    for (size_t i = 0; i < total_pairs; ++i)
    {
      elem_rates[i].resize(_collision_objects[i].size());
    }
  }
}

unsigned int
ParticleColliderBase::pairingFunction(const unsigned int species_id_1,
                                      const unsigned int species_id_2) const
{
  const auto helper = [](const unsigned int n) -> unsigned int { return n * (n + 1) / 2; };
  return helper(std::max(species_id_1, species_id_2)) + std::min(species_id_1, species_id_2);
}

unsigned int
ParticleColliderBase::totalUniquePairs(const unsigned int num_species) const
{
  return pairingFunction(num_species, num_species) + 1;
}
