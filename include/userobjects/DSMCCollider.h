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

#include "CollisionBase.h"
#include "ParticleColliderBase.h"

class DSMCCollider : public ParticleColliderBase
{
public:
  DSMCCollider(const InputParameters & parameters);

  static InputParameters validParams();

  virtual void
  initializeInternalData(const std::vector<std::shared_ptr<Ray>> & particles) const override;
  virtual void collideParticles(const std::vector<std::shared_ptr<Ray>> & particles) const override;

protected:
  /// this will store the maximum value of the produce of
  /// the cross section and relative speed
  /// the first index will be the element and
  /// the second vector will store the value for each species pair
  mutable std::vector<std::vector<Real>> _elem_wise_sigma_cr_t_max;

  /**
   * Performs particle particle collisions for a set of particles in a given element
   * for two species: Species A and Species B
   * @param particles the list of all of the particles in the simulation
   * @param a_indicies the list of indicies into `particles` for particles of species_a
   * @param b_indicies the list of indicies into `particles` for particles of species_b
   * @param collisions the list of collision pairs for these the particle pairs
   * @param elem_volume the volume of the element in which the particles are currently colliding
   * @param same_species this tells the system wether or not the particles are of the same species
   * or not
   * @param sigma_cr_t_max the maximum value of the total cross section times the realtive speed
   * encountered on the previous time step
   * @param reaction_rates a vector where the reaction rates for all reactions on this time step
   * will be stored
   * @returns the maximum value of the total cross section times the realtive speed encountered
   * during this round of collisions
   * */
  const Real collideSpeciesPair(const std::vector<std::shared_ptr<Ray>> & particles,
                                const std::vector<size_t> & a_indicies,
                                const std::vector<size_t> & b_indicies,
                                const std::vector<const CollisionBase *> & collisions,
                                const Real elem_volume,
                                const bool same_species,
                                const Real & sigma_cr_t_max_prev,
                                std::vector<Real> & reaction_rates) const;
};
