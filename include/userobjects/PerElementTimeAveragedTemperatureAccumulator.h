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

class PICStudyBase;
/**
 * UserObject for testing the AuxAccumulator
 */
class PerElementTimeAveragedTemperatureAccumulator : public GeneralUserObject
{
public:
  static InputParameters validParams();

  PerElementTimeAveragedTemperatureAccumulator(const InputParameters & params);

  virtual void initialize() override {}
  virtual void execute() override;
  virtual void finalize() override {}

protected:
  const PICStudyBase & _study;
  const unsigned int _species_id;
  const unsigned int _time_step_start;
  std::vector<Real> _total_weight;
  std::vector<Point> _total_velocity;
  std::vector<Real> _total_speed;
};
