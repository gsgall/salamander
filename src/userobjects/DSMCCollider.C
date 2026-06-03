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
#include "ParticleColliderBase.h"
#include "Ray.h"
#include <petscmat.h>

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
DSMCCollider::setupInternalData()
{

  for (const auto & elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    _elem_ids.push_back(elem->id());
    _elem_volumes.push_back(elem->volume());
  }

  _element_wise_max_rates.resize(_elem_ids.size());
  _particle_indicies.resize(_species_ids.size());

  const auto total_pairs = pairingFunction(_species_ids.size() - 1, _species_ids.size() - 1) + 1;
  for (size_t i = 0; i < _elem_ids.size(); ++i)
  {
    _element_wise_max_rates[i].resize(total_pairs);
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
    /// iterator over all of the different types of pairs
    const auto elem_max_rates = _element_wise_max_rates[i];
    const auto elem_volume = _elem_volumes[i];
    for (size_t j = 0; j < elem_max_rates.size(); ++j)
    {
      for (const auto id_a : _species_ids)
      {
        for (const auto id_b : _species_ids)
        {
          const auto pair_index = pairingFunction(id_a, id_b);
          const auto sigma_cr_max = _element_wise_max_rates[i][pair_index];

          const Real a_count = _particle_indicies[id_a].size();
          const Real b_count = _particle_indicies[id_b].size();
          const Real unique_pairs = 0.5 * (a_count * id_a == id_b ? a_count - 1 : b_count);

          unsigned int collision_pairs = static_cast<unsigned int>(
              unique_pairs * sigma_cr_max * particle_weight * _dt / elem_volume +
              _generator.rand());

          for (size_t k = 0; k < collision_pairs; ++k)
          {
          }
        }
      }
    }
  }
}
