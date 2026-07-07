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

class PICStudy;
class CollisionBase : public GeneralUserObject
{
public:
  CollisionBase(const InputParameters & parameters);

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
  ///

  /**
   * Method for computing the cross section times the
   * relative velocity of the particles
   * @param particle_a the first particle of the colliding pair
   * @param particle_b the second particle of the colliding pair
   * @return the cross section multiplied by the relative velocity of the particles
   */
  virtual const Real sigmaCr(const Ray & particle_a, const Ray & particle_b) const = 0;

  /**
   * Based on the current state of particle in the system an estimation for the
   * maximum value of the cross section times the relative velocity for the system
   * will be estimated
   */
  virtual const Real
  estimateSigmaCrMax(const std::vector<std::shared_ptr<Ray>> & particles) const = 0;

  /**
   * Method for modifying the internal particle data post collision
   * @param particle_a the first particle of the colliding pair
   * @param particle_b the second particle of the colliding pair
   */
  virtual void collideParticles(Ray & particle_a, Ray & particle_b) const = 0;

  /**
   * getter methods for the species ids of either the reactants of the
   * products after the collision
   * @returns a list of the species involed on the respective side of the reaction
   */
  ///@{
  const std::array<unsigned int, 2> & reactantIds() const;
  const std::array<unsigned int, 2> & productIds() const;
  ///@}

protected:
  /// random number generator used if a random number is needed during the collision
  MooseRandom _generator;
  /// the study the owns the particles
  const PICStudy * _study;
  /// internal storage for reactants and products for the species in the reaction
  ///@{
  std::array<unsigned int, 2> _reactant_ids;
  std::array<unsigned int, 2> _product_ids;
  ///@}
};
