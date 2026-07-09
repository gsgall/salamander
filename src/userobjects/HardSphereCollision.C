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

#include "HardSphereCollisionBase.h"
#include "HardSphereCollision.h"
#include "Constants.h"
#include "PICStudy.h"

registerMooseObject("SalamanderApp", HardSphereCollision);

InputParameters
HardSphereCollision::validParams()
{
  auto params = HardSphereCollisionBase::validParams();
  params.addClassDescription("Base class to handle elastic collisions");

  params.addParam<Real>("diameter",
                        "The effective molecular diameter for the cross section calculation");

  return params;
}

HardSphereCollision::HardSphereCollision(const InputParameters & parameters)
  : HardSphereCollisionBase(parameters), _sigma(M_PI * std::pow(getParam<Real>("diameter"), 2.0))
{
}

const Real
HardSphereCollision::sigmaCr(const Ray & particle_a, const Ray & particle_b) const
{
  return _study->relativeSpeed(particle_a, particle_b) * _sigma;
}

const Real
HardSphereCollision::estimateSigmaCrMax(const std::vector<std::shared_ptr<Ray>> & particles) const
{
  const auto mass = _study->mass(*particles.front());
  Real total_energy = 0;
  Real total_weight = 0;

  for (const auto & particle : particles)
  {
    const auto weight = _study->weight(*particle);
    total_weight += weight;
    total_energy += weight * _study->energy(*particle);
  }

  const auto temperature = 2.0 * total_energy / (total_weight * 3.0 * Salamander::constants::k_b);

  const auto cr_max_est =
      2 * std::sqrt(2) * std::sqrt(8 * Salamander::constants::k_b * temperature / (M_PI * mass));

  return _sigma * cr_max_est;
}
