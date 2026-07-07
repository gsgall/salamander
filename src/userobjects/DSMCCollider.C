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

#include "PICStudy.h"
#include "DSMCCollider.h"
#include "CollisionBase.h"
#include "libMeshReducedNamespace.h"

registerMooseObject("SalamanderApp", DSMCCollider);

InputParameters
DSMCCollider::validParams()
{
  auto params = ParticleColliderBase::validParams();
  params.addClassDescription("Base class for ParticleStepper. Provides the basic implementation"
                             "for dimensional dependent velocity updating."
                             "And the ability to sample vector fields for use in a particle step");
  return params;
}

DSMCCollider::DSMCCollider(const InputParameters & parameters) : ParticleColliderBase(parameters) {}

void
DSMCCollider::initializeInternalData(const std::vector<std::shared_ptr<Ray>> & particles) const
{
  const auto num_species = _study->speciesNames().size();
  unsigned int max_collisions = 0;
  for (const auto & collisions : _collision_objects)
  {
    if (collisions.size() > max_collisions)
    {
      max_collisions = collisions.size();
    }
  }
  _elem_wise_sigma_cr_t_max.resize(_elem_ids.size());

  const auto total_pairs = totalSpeciesPairs(num_species);

  for (size_t i = 0; i < _elem_ids.size(); ++i)
  {
    _elem_wise_sigma_cr_t_max[i].resize(total_pairs);
  }

  for (auto & elem_sigma_cr_t_maxes : _elem_wise_sigma_cr_t_max)
  {
    for (size_t pair_index = 0; pair_index < total_pairs; ++pair_index)
    {
      auto & sigma_cr_t_max = elem_sigma_cr_t_maxes[pair_index];
      for (const auto & collision : _collision_objects[pair_index])
      {
        sigma_cr_t_max += collision->estimateSigmaCrMax(particles);
      }
      elem_sigma_cr_t_maxes[pair_index] = sigma_cr_t_max;
    }
  }
}

void
DSMCCollider::collideParticles(const std::vector<std::shared_ptr<Ray>> & particles) const
{

  for (size_t element_index = 0; element_index < _elem_ids.size(); ++element_index)
  {
    const auto elem_id = _elem_ids[element_index];
    auto & elem_reaction_rates = _reaction_rates[element_index];

    setParticleIndicies(particles, elem_id, _particle_indicies);

    const auto elem_volume = _elem_volumes[element_index];
    auto & elem_sigma_cr_t_max = _elem_wise_sigma_cr_t_max[element_index];

    for (const auto id_a : make_range(_species_count))
    {
      for (size_t id_b = id_a; id_b < _species_count; ++id_b)
      {
        const auto & a_indicies = _particle_indicies[id_a];
        const auto & b_indicies = _particle_indicies[id_b];

        const auto pair_index = pairingFunction(id_a, id_b);

        const auto & collisions = _collision_objects[pair_index];

        auto & reaction_rates = elem_reaction_rates[pair_index];
        auto & sigma_cr_t_max = elem_sigma_cr_t_max[pair_index];

        const bool same_species = id_a == id_b;
        sigma_cr_t_max = collideSpeciesPair(particles,
                                            a_indicies,
                                            b_indicies,
                                            collisions,
                                            elem_volume,
                                            same_species,
                                            sigma_cr_t_max,
                                            reaction_rates);
      }
    }
  }
}

const Real
DSMCCollider::collideSpeciesPair(const std::vector<std::shared_ptr<Ray>> & particles,
                                 const std::vector<size_t> & a_indicies,
                                 const std::vector<size_t> & b_indicies,
                                 const std::vector<const CollisionBase *> & collisions,
                                 const Real elem_volume,
                                 const bool same_species,
                                 const Real & sigma_cr_t_max_prev,
                                 std::vector<Real> & reaction_rates) const
{

  Real sigma_cr_t_max_curr = 0.0;

  for (auto & rate : reaction_rates)
  {
    rate = 0;
  }

  // This currently assumes a fixed weight scheme.
  const Real weight = _study->weight(*particles.front());

  const Real a_count = a_indicies.size();
  const Real b_count = b_indicies.size();

  const Real unique_pairs = (a_count * (same_species ? (a_count - 1) / 2.0 : b_count));

  const unsigned int num_trials = static_cast<unsigned int>(
      unique_pairs * sigma_cr_t_max_prev * weight * _dt / elem_volume + _generator.rand());

  for (size_t i = 0; i < num_trials; ++i)
  {
    const auto index_a = static_cast<size_t>(a_count * _generator.rand());
    size_t index_b;
    do
    {
      index_b = static_cast<size_t>(b_count * _generator.rand());
    } while ((same_species) && (index_a == index_b));

    const auto particle_a = particles[a_indicies[index_a]];
    const auto particle_b = particles[b_indicies[index_b]];

    mooseAssert(_study->weight(*particle_a) == _study->weight(*particle_b),
                "Mixed weight particle collision schemes are currently not supported");

    Real sigma_cr_t = 0.0;

    for (const auto & collision : collisions)
    {
      sigma_cr_t += collision->sigmaCr(*particle_a, *particle_b);
    }

    if (sigma_cr_t > sigma_cr_t_max_curr)
    {
      sigma_cr_t_max_curr = sigma_cr_t;
    }

    if (sigma_cr_t / sigma_cr_t_max_prev < _generator.rand())
    {
      continue;
    }

    const auto selection_rand = _generator.rand();

    size_t collision_index = 0;
    for (size_t j = 0; j < collisions.size(); ++j)
    {
      collision_index += static_cast<size_t>(selection_rand > selection_rand);
    }

    collisions[collision_index]->collideParticles(*particle_a, *particle_b);
    reaction_rates[collision_index] += weight;
  }

  for (auto & rate : reaction_rates)
  {
    rate /= (elem_volume * _dt);
  }

  return sigma_cr_t_max_curr;
}
