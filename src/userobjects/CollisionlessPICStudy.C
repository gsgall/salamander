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

#include "CollisionlessPICStudy.h"

registerMooseObject("SalamanderApp", CollisionlessPICStudy);

InputParameters
CollisionlessPICStudy::validParams()
{
  auto params = PICStudyBase::validParams();
  params.addClassDescription("PIC Study Object for Collisionless PIC simulations");
  return params;
}

CollisionlessPICStudy::CollisionlessPICStudy(const InputParameters & parameters)
  : PICStudyBase(parameters)
{
}

void
CollisionlessPICStudy::initializeParticles()
{
  PICStudyBase::initializeParticles();
  moveRaysToBuffer(_banked_rays);
}
