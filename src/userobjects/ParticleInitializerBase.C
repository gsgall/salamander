//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic confinement for Numerical Design, Engineering & Research,
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
//* Copyright 2025, Battelle Energy Alliance, LLC
//* ALL RIGHTS RESERVED
//*

#include "ParticleInitializerBase.h"
#include "Distribution.h"

InputParameters
ParticleInitializerBase::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addClassDescription("Base class for ParticleStepper. Provides the basic implementation"
                             "for dimensional dependent velocity updating."
                             "And the ability to sample vector fields for use in a particle step");
  params.addParam<unsigned int>("seed", 0, "An additional seed for the random number generators");
  params.addRangeCheckedParam<Real>(
      "mass", 1.0, "mass > 0.0", "The mass of the particles being placed in the mesh");
  params.addParam<Real>("charge", 1, "The charge of the particles being placed in the mesh");
  params.addParam<std::string>("species", "", "The type of particle that is being initialized");
  params.addRequiredParam<std::vector<DistributionName>>(
      "velocity_distributions",
      "The distribution names to be sampled when initializing the velocity of each particle");
  params.addParam<bool>("sample_velocity_magnitude", false, "Whether or not you want to sample each velocity component seperately or just sample the magnitude");
  return params;
}

ParticleInitializerBase::ParticleInitializerBase(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    _mass(getParam<Real>("mass")),
    _charge(getParam<Real>("charge")),
    _species(getParam<std::string>("species")),
    _seed(getParam<unsigned int>("seed")),
    _mesh_dimension(_fe_problem.mesh().dimension()),
    _sample_magnitude(getParam<bool>("sample_velocity_magnitude")),
    _distribution_names(getParam<std::vector<DistributionName>>("velocity_distributions"))
{
  if (_sample_magnitude)
  {
    if (_distribution_names.size() != 1)
      paramError("velocity_distributions",
                 "Only one distribution may be provided when sampling the velocity magnitude.");
    return;
  }

  if (_distribution_names.size() != 3)
    paramError("velocity_distributions",
               "You must provide 3 distributions, one for each velocity component.");
}

void
ParticleInitializerBase::initialSetup()
{
  // Needed because distributions are constructed after UserObjects
  for (const DistributionName & name : _distribution_names)
    _velocity_distributions.push_back(&getDistributionByName(name));
}
