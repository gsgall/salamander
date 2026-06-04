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
#include "Ray.h"

class PICStudyBase;
/**
 * UserObject for testing the AuxAccumulator
 */
class PerElementAverageTemperatureAccumulator : public GeneralUserObject
{
public:
  static InputParameters validParams();

  PerElementAverageTemperatureAccumulator(const InputParameters & params);

  virtual void initialize() override {}
  virtual void execute() override;
  virtual void finalize() override {}

protected:
  std::vector<RayData> _total_energy_per_cell;
  std::vector<RayData> _total_weights_per_cell;

  const PICStudyBase & _study;
  const unsigned int _species_id;
  const std::vector<RayDataIndex> _velocity_indicies;
  const RayDataIndex _species_index;
  const RayDataIndex _mass_index;
  const RayDataIndex _weight_index;
};
