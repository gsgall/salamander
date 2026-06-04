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

#include "CollisionalPICStudy.h"
#include "PICStudyBase.h"
#include "ParticleColliderBase.h"

registerMooseObject("SalamanderApp", CollisionalPICStudy);

InputParameters
CollisionalPICStudy::validParams()
{
  auto params = PICStudyBase::validParams();
  params.addClassDescription("PIC Study Class that implements the logic required for performing "
                             "the DSMC collisional algorithm.");
  params.addRequiredParam<UserObjectName>(
      "collider", "The object that contains the logic for particle collisions");

  return params;
}

CollisionalPICStudy::CollisionalPICStudy(const InputParameters & parameters)
  : PICStudyBase(parameters)
{
}

void
CollisionalPICStudy::initialSetup()
{
  PICStudyBase::initialSetup();
  _collider = &const_cast<ParticleColliderBase &>(getUserObject<ParticleColliderBase>("collider"));
}

void
CollisionalPICStudy::initializeParticles()
{
  PICStudyBase::initializeParticles();
  _collider->initializeInternalData(_banked_rays);
  moveRaysToBuffer(_banked_rays);
}

void
CollisionalPICStudy::postExecuteStudy()
{
  PICStudyBase::postExecuteStudy();

  /// we are going to sort all of the particles we have by the element
  /// if for the elements that end in for collisions
  std::sort(_banked_rays.begin(),
            _banked_rays.end(),
            [](const std::shared_ptr<Ray> & a, const std::shared_ptr<Ray> & b)
            { return a->currentElem()->id() < b->currentElem()->id(); });

  _collider->collideParticles(_banked_rays);
}
