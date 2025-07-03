//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic
//confinement for Numerical Design, Engineering & Research,
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

#include "GenericRayKernel.h"

template <bool is_ad>
class CurrentDensityRayKernelTempl : public GenericRayKernel<is_ad>
{
public:
  CurrentDensityRayKernelTempl(const InputParameters & params);

  static InputParameters validParams();

protected:
  virtual GenericReal<is_ad> computeQpResidual() override;
  /// the component of the velocity vector used for the current density calculation
  const unsigned int _component;
  /// the index for where the charge of the particle is stored in ray data
  const RayDataIndex _charge_index;
  /// the index for where the weight of the computational particle is stored in ray data
  const RayDataIndex _weight_index;
  /// the actual ray data index corrisponding to the velocity component selected is stored in ray data
  RayDataIndex _velocity_index;

  usingGenericRayKernelMembers;
};

typedef CurrentDensityRayKernelTempl<true> ADCurrentDensityRayKernel;
