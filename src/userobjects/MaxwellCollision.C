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

#include "MaxwellCollision.h"

registerMooseObject("SalamanderApp", MaxwellCollision);

InputParameters
MaxwellCollision::validParams()
{
  auto params = ElasticCollisionBase::validParams();
  params.addClassDescription("Elastic Isotropic Collisions between maxwell molecules");
  params.addRequiredRangeCheckedParam<Real>(
      "reference_value",
      "reference_value > 0",
      "The reference cross section value for maxwell molecule interactions");

  return params;
}

MaxwellCollision::MaxwellCollision(const InputParameters & parameters)
  : ElasticCollisionBase(parameters), _sigma_ref(getParam<Real>("reference_value"))
{
}

const Real
MaxwellCollision::estimateSigmaCrMax(const std::vector<std::shared_ptr<Ray>> & /*particles*/) const
{
  return _sigma_ref;
}

const Real

MaxwellCollision::sigmaCr(const Ray & particle_a, const Ray & particle_b) const
{
  return _sigma_ref;
}
