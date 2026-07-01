//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic
// confinement for Numerical Design, Engineering & Research,
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

#include "ParticleStepperBase.h"
#include "TestStationaryStepper.h"
#include "Ray.h"

registerMooseObject("SalamanderTestApp", TestStationaryStepper);

InputParameters
TestStationaryStepper::validParams()
{
  auto params = ParticleStepperBase::validParams();
  params.addClassDescription("This particle stepper sets all of the particles to staionary so that "
                             "they are not moving regardless of particle velocity");
  return params;
}

TestStationaryStepper::TestStationaryStepper(const InputParameters & parameters)
  : ParticleStepperBase(parameters)
{
}

void
TestStationaryStepper::setupStep(Ray & particle,
                                 Point & /*v*/,
                                 const Real /*q_m_ratio*/,
                                 const Real /*disatnce*/) const
{
  particle.setStationary();
}
