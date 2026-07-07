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

#include "AuxAccumulator.h"
#include "GeneralUserObject.h"
#include "MooseRandom.h"
#include <libmesh/id_types.h>

class CollisionBase;
class Ray;
class PICStudy;

struct CollisionIndices
{
  unsigned int pair_index;
  unsigned int collision_index;
};

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

  /**
   * This should be called after the study creates initial condition particles
   * This will allow different collider objects do some initial setup if they need to
   * For example in DSMC before the first time step we need an initial estimate certain
   * parameters so that on the first actual round of collisions it is ready
   */
  virtual void
  initializeInternalData(const std::vector<std::shared_ptr<Ray>> & particles) const = 0;
  /**
   * The method that should be called to do the collisional algorithm on the list of particles in
   * the system
   * @param the list of particles that will collide
   */
  virtual void collideParticles(const std::vector<std::shared_ptr<Ray>> & particles) const = 0;

  const CollisionIndices collisionIndices(const std::string_view collision_name) const;

  void fillAuxAccumulator(const CollisionIndices indicies,
                          SALAMANDER::AuxAccumulator & accumulator) const;

protected:
  /// the random number generator that would be used during a collisional method
  MooseRandom _generator;
  /// the study that owns the particles
  const PICStudy * _study;
  /// the number of unique species in the system
  unsigned int _species_count;
  /**
   * a doubly indexed list to store inidicies into the particle list
   * for particles in a given element
   * the first index is the species id
   * after being indexed by species there is a list of indicies for particles of the given species
   * in an element
   */
  mutable std::vector<std::vector<size_t>> _particle_indicies;
  /**
   * the list of all collisions that could occur
   * the first index is the speices pairing index for a given
   * species pairing this is produced by the pairing function method
   */
  std::vector<std::vector<const CollisionBase *>> _collision_objects;
  /**
   * Storage for the reaction rates of each type in each element
   * the first index into the vector is the element
   * the second index is the pair index for the species pair
   * the third index is the index of a given reaction for a given species pair
   */
  mutable std::vector<std::vector<std::vector<Real>>> _reaction_rates;
  /// The list of the volume of each locally owned element
  std::vector<Real> _elem_volumes;
  /// The list of the element ids for each locally own element
  std::vector<dof_id_type> _elem_ids;

  /**
   * This provides a unique id for a particle id pairing
   * species ids should be in the range [0, n - 1] where n is
   * the number of species in the system
   * This pairing function is symmetric and densly packs the values
   * For 3 species this would be the pairing function outputs
   * 0 | 0 | 1 | 2
   * --------------
   * 0 | 0 | 1 | 3
   * 1 | 1 | 2 | 4
   * 2 | 3 | 4 | 5
   * @param species_id_1 species id of the first particle
   * @param species_id_2 species id of the second particle
   * @returns the pairing id for the two particles
   */
  unsigned int pairingFunction(const unsigned int species_id_1,
                               const unsigned int species_id_2) const;

  /**
   * The total number of combinations of all species that exist in the system
   * @param the total number of species that the study knows about
   * @returns the total number of combinations of species in the system
   */
  unsigned int totalSpeciesPairs(const unsigned int num_species) const;

  /**
   * Collects the indicies into the particle list for all of the particles that
   * are presents in a given element id
   * the first index of the indicies vector is the particle species id
   * for each species there is a list of indicies into the particles vector
   * @param elem_id the id of the element for which particles should be collected
   * @param particles the list of particles in the simulation
   * @param indicies the vector the indicies will be collected into
   */
  void setParticleIndicies(const std::vector<std::shared_ptr<Ray>> & particles,
                           const dof_id_type elem_id,
                           std::vector<std::vector<size_t>> & indicies) const;
};
