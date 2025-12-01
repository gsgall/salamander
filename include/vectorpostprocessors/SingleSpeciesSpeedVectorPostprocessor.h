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
#include "Ray.h"
#include "GeneralVectorPostprocessor.h"
// Forward declarations
class PICStudyBase;

class SingleSpeciesSpeedVectorPostprocessor : public GeneralVectorPostprocessor
{
public:
  static InputParameters validParams();

  SingleSpeciesSpeedVectorPostprocessor(const InputParameters & parameters);

  /**
   * clears the data vector postprocessors before each
   * execution
   */
  virtual void initialize() override;
  /**
   * collects all of the data requested from particles
   */
  virtual void execute() override;
  /**
   * The finalize method simply communicates the data that exists on
   * individual processors to the root processor
   */
  virtual void finalize() override;

protected:
  /// the study that we are collecting data from
  const PICStudyBase & _study;
  const RayDataIndex _species_index;
  const RayDataIndex _v_x_index;
  const RayDataIndex _v_y_index;
  const RayDataIndex _v_z_index;
  const size_t _species_id;
  VectorPostprocessorValue * _speeds;
};
