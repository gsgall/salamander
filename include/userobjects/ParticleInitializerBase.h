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
#include "ParticleData.h"

class VelocityInitializerBase;

class ParticleInitializerBase : public GeneralUserObject
{
public:
  ParticleInitializerBase(const InputParameters & parameters);

  static InputParameters validParams();

  /**
   * This method will be called by PIC studies to give them all of the data needed to create and
   * place particles on the finite element mesh
   * */
  virtual std::vector<Salamander::ParticleData> getParticleData() const = 0;

  /**
   * This method allows the PIC study to retrieve the name of the species that this initializer is
   * responsible for.
   */
  const std::string & speciesName() const { return _species; }

  /**
   * Unused methods
   */
  ///@{
  virtual void initialize() override final {}
  virtual void finalize() override final {}
  virtual void execute() override final {}
  ///@}

protected:
  /// the mass of the particles being created
  const Real _mass;
  /// the charge of the particles being created
  const Real _charge;
  /// the type of particle being created
  const std::string & _species;
  /// a user specified seed for changing random number generator seeds
  const unsigned int _seed;
  /// the dimension of the finite element mesh
  const Real _mesh_dimension;
  /// The velocity initializer which will give all particles their initial velocity distribution
  const VelocityInitializerBase & _velocity_initializer;
};
