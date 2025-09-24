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

#include "MooseRandom.h"
#include "PICStudyBase.h"

class CollisionalPICStudy : public PICStudyBase
{
public:
  CollisionalPICStudy(const InputParameters & parameters);

  static InputParameters validParams();

  virtual void postExecuteStudy() override;

protected:
  const std::vector<Real> _cross_sections;
  std::vector<std::vector<unsigned int>> _particle_indicies;
  MooseRandom _generator;
};
