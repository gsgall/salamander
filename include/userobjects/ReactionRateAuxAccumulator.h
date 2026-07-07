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

#pragma once

#include "GeneralUserObject.h"
#include "ParticleColliderBase.h"

class PICStudy;

class ReactionRateAuxAccumulator : public GeneralUserObject
{
public:
  static InputParameters validParams();

  ReactionRateAuxAccumulator(const InputParameters & params);

  virtual void initialSetup() override;
  virtual void initialize() override {}
  virtual void execute() override;
  virtual void finalize() override {}

protected:
  const PICStudy & _study;
  const AuxVariableName & _aux_variable;
  const ParticleColliderBase & _collider;
  CollisionIndices _indicies;
};
