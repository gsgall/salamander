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
#include "PICStudy.h"
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
  for (const auto & elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    _elem_volumes.push_back(elem->volume());
    _elem_ids.push_back(elem->id());
  }
}

void
ParticleColliderBase::initialSetup()
{
  _study = &getUserObject<PICStudy>("study");
  _species_count = _study->speciesNames().size();

  _particle_indicies.resize(_species_count);
  const auto & names = getParam<std::vector<UserObjectName>>("collision_objects");

  const auto total_pairs = totalSpeciesPairs(_species_count);

  _collision_objects.resize(total_pairs);

  for (const auto & name : names)
  {
    const auto & collision = getUserObjectByName<CollisionBase>(name);
    const auto reactant_ids = collision.reactantIds();
    const auto pair_id = pairingFunction(reactant_ids[0], reactant_ids[1]);
    _collision_objects[pair_id].push_back(&collision);
  }

  _reaction_rates.resize(_elem_volumes.size());

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
ParticleColliderBase::totalSpeciesPairs(const unsigned int num_species) const
{
  return pairingFunction(num_species - 1, num_species - 1) + 1;
}

void
ParticleColliderBase::setParticleIndicies(const std::vector<std::shared_ptr<Ray>> & particles,
                                          const dof_id_type elem_id,
                                          std::vector<std::vector<size_t>> & indicies) const
{
  /// make sure that all of the values are cleared so we do not accumulate over time
  for (auto & species_indicies : indicies)
  {
    species_indicies.clear();
  }

  mooseAssert(indicies.size() == _study->speciesNames().size(),
              "The particle indicies vector size was not properly set");

  for (size_t i = 0; i < particles.size(); ++i)
  {
    const auto & particle = particles[i];
    if (particle->currentElem()->id() != elem_id)
      continue;

    indicies[_study->species(*particle)].push_back(i);
  }
}
