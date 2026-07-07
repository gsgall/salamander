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
#include "MaxwellHardSphereCollision.h"

registerMooseObject("SalamanderApp", MaxwellHardSphereCollision);

InputParameters
MaxwellHardSphereCollision::validParams()
{
  auto params = HardSphereCollisionBase::validParams();
  params.addClassDescription("Base class to handle elastic collisions");

  params.addParam<Real>("sigma_ref", "The reference cross section of the Maxwell cross section");

  return params;
}

MaxwellHardSphereCollision::MaxwellHardSphereCollision(const InputParameters & parameters)
  : HardSphereCollisionBase(parameters), _sigma_ref(getParam<Real>("sigma_ref"))
{
}

const Real
MaxwellHardSphereCollision::sigmaCr(const Ray & /*particle_a*/, const Ray & /*particle_b*/) const
{
  return _sigma_ref;
}

const Real
MaxwellHardSphereCollision::estimateSigmaCrMax(
    const std::vector<std::shared_ptr<Ray>> & /*particles*/) const
{
  return _sigma_ref;
}
