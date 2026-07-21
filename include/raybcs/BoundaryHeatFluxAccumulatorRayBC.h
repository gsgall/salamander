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
//* Copyright 2025, Battelle Energy Alliance, LLC and North Carolina State University
//* ALL RIGHTS RESERVED
//*

#pragma once

#include "RayBoundaryConditionBase.h"

class PICStudy;
namespace SALAMANDER
{
class NonZeroedAuxAccumulator;
};

class BoundaryHeatFluxAccumulatorRayBC : public RayBoundaryConditionBase
{
public:
  BoundaryHeatFluxAccumulatorRayBC(const InputParameters & params);

  static InputParameters validParams();
  virtual void timestepSetup() override;
  virtual void preOnBoundary(const unsigned int num_applying) override;
  virtual void onBoundary(const unsigned int /*num_applying*/) override {};
  virtual void postOnBoundary(const unsigned int num_applying) override;
  virtual void postExecuteStudy() override;

protected:
  const PICStudy & _pic_study;
  const AuxVariableName & _aux_variable;
  const unsigned int _time_step_start;
  const unsigned int _mesh_dimension;
  Real _sum_time;
  Point _temporary_velocity;

  std::unique_ptr<SALAMANDER::NonZeroedAuxAccumulator> accumulator;

  const Real directionalEnergy();
};
