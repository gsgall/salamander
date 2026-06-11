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

#include "HardSphereCollision.h"
#include "CollisionalPICStudy.h"
#include "Constants.h"

registerMooseObject("SalamanderApp", HardSphereCollision);

InputParameters
HardSphereCollision::validParams()
{
  auto params = ElasticCollisionBase::validParams();
  params.addClassDescription("Elastic Isotropic Collisions between maxwell molecules");
  params.addRequiredRangeCheckedParam<Real>(
      "diameter", "diameter > 0", "Hard sphere collisions based on a molecular diameter.");
  params.addRequiredRangeCheckedParam<Real>(
      "initial_temperature",
      "initial_temperature > 0",
      "The initial temperature estimate of the particles. This will be used to estimate the "
      "initial value of sigma cr max.");
  return params;
}

HardSphereCollision::HardSphereCollision(const InputParameters & parameters)
  : ElasticCollisionBase(parameters), _sigma_total(M_PI * std::pow(getParam<Real>("diameter"), 2))
{
}

const Real
HardSphereCollision::estimateSigmaCrMax(const std::vector<std::shared_ptr<Ray>> & particles) const
{
  // assuming for now that they all have the same mass
  const auto mass = _study->mass(*particles.front());
  const auto temperature = getParam<Real>("initial_temperature");

  // setting the initial estimate as twice the most probable relative speed
  const auto cr_max_est =
      2 * std::sqrt(2) * std::sqrt(8 * Salamander::constants::k_b * temperature / (M_PI * mass));
  return _sigma_total * cr_max_est;
}

const Real

HardSphereCollision::sigmaCr(const Ray & particle_a, const Ray & particle_b) const
{
  return _sigma_total * relativeSpeed(particle_a, particle_b);
}
