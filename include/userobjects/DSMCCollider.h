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

  virtual void initializeInternalData(const std::vector<std::shared_ptr<Ray>> & particles) override;
  virtual void collideParticles(const std::vector<std::shared_ptr<Ray>> & particles) override;

protected:
  /// this will store the maximum value of the produce of
  /// the cross section and relative speed
  /// the first index will be the element and
  /// the second vector will store the value for each species pair
  std::vector<std::vector<Real>> _elem_wise_max_cr_values;
  std::vector<Real> _temporary_sigma_cr_values;

  void collideSpeciesPair(const std::vector<std::shared_ptr<Ray>> & particles,
                          const std::vector<size_t> & a_indicies,
                          const std::vector<size_t> & b_indicies,
                          const std::vector<const CollisionBase *> & colliders,
                          const Real elem_volume,
                          const bool same_species,
                          std::vector<Real> & reaction_rates,
                          Real & sigma_cr_t_max);
};
