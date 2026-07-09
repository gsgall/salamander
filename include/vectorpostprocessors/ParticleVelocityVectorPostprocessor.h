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
#include "GeneralVectorPostprocessor.h"

class PICStudy;

class ParticleVelocityVectorPostprocessor : public GeneralVectorPostprocessor
{
public:
  static InputParameters validParams();

  ParticleVelocityVectorPostprocessor(const InputParameters & parameters);

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
  const PICStudy & _study;
  /// vector postprocessor values
  std::vector<VectorPostprocessorValue *> _data_values;
};
