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

#include "DSMCCollider.h"
#include "CollisionBase.h"
#include "ParticleColliderBase.h"
#include "Ray.h"
#include <utility>

InputParameters
DSMCCollider::validParams()
{
  auto params = ParticleColliderBase::validParams();
  params.addClassDescription("Base class for ParticleStepper. Provides the basic implementation"
                             "for dimensional dependent velocity updating."
                             "And the ability to sample vector fields for use in a particle step");
  return params;
}

DSMCCollider::DSMCCollider(const InputParameters & parameters) : ParticleColliderBase(parameters)
{

  _particle_indicies.resize(_species_ids.size());

  for (const auto & elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    _elem_volumes.push_back(elem->volume());
    _elem_ids.push_back(elem->id());
  }
  _elem_wise_max_cr_values.resize(_elem_ids.size());

  const auto total_pairs = pairingFunction(_species_ids.size(), _species_ids.size()) + 1;

  for (size_t i = 0; i < _elem_ids.size(); ++i)
  {
    _elem_wise_max_cr_values[i].resize(total_pairs);
  }
}

void
DSMCCollider::collideParticles(const std::vector<std::shared_ptr<Ray>> & particles)
{
  size_t curr_particle_index = 0;
  const auto particle_weight = particles.front()->data(_weight_index);

  for (size_t i = 0; i < _elem_ids.size(); ++i)
  {

    for (size_t j = 0; j < _species_ids.size(); ++j)
    {
      _particle_indicies[j].clear();
    }

    const auto curr_particle = particles[curr_particle_index];
    /// this does assume that the particles are in sorted order when we get them from the study
    /// the collisional study base should ensure this is true
    while (curr_particle->currentElem()->id() == _elem_ids[i])
    {
      _particle_indicies[curr_particle->data(_species_index)].push_back(curr_particle_index);
      curr_particle_index++;
    }

    const auto elem_volume = _elem_volumes[i];
    auto & elem_max_rates = _elem_wise_max_cr_values[i];
    /// iterator over all of the different types of pairs
    for (size_t j = 0; j < elem_max_rates.size(); ++j)
    {
      for (const auto id_a : _species_ids)
      {
        for (const auto id_b : _species_ids)
        {
          const auto pair_index = pairingFunction(id_a, id_b);
          auto & sigma_cr_max = elem_max_rates[pair_index];

          const Real a_count = _particle_indicies[id_a].size();
          const Real b_count = _particle_indicies[id_b].size();

          const Real unique_pairs = 0.5 * (a_count * (id_a == id_b ? a_count - 1 : b_count));

          unsigned int collision_pairs = static_cast<unsigned int>(
              unique_pairs * sigma_cr_max * particle_weight * _dt / elem_volume +
              _generator.rand());

          const auto & collisions = _collision_objects[pair_index];
          auto & temp_xsecs = _temporary_xsecs[pair_index];

          for (size_t k = 0; k < collision_pairs; ++k)
          {
            const auto index_a = static_cast<size_t>(a_count * _generator.rand());
            const auto index_b = static_cast<size_t>(b_count * _generator.rand());

            const auto particle_a = particles[index_a];
            const auto particle_b = particles[index_b];

            Real total_xsec = 0;
            for (size_t l = 0; l < collisions.size(); ++l)
            {
              const auto xsec_val = collisions[l]->sampleCrossSection(*particle_a, *particle_b);
              temp_xsecs[l] = xsec_val;
              total_xsec += xsec_val;
            }
            const auto selection_rand = _generator.rand();
            size_t collision_index = 0;

            for (size_t l = 0; l < collisions.size(); ++l)
            {
              collision_index += static_cast<size_t>(selection_rand < temp_xsecs[l] / total_xsec);
            }

            const auto & collision = collisions[collision_index];
            const auto sigma_cr =
                temp_xsecs[collision_index] * collision->relativeSpeed(*particle_a, *particle_b);

            if (sigma_cr > sigma_cr_max)
            {
              sigma_cr_max = sigma_cr;
            }

            if (sigma_cr / sigma_cr_max < _generator.rand())
            {
              continue;
            }

            collisions[collision_index]->collideParticles(*particle_a, *particle_b);
          }
        }
      }
    }
  }
}
