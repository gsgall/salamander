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

#pragma once

#include "ParticleColliderBase.h"

class DSMCCollider : public ParticleColliderBase
{
public:
  DSMCCollider(const InputParameters & parameters);

  static InputParameters validParams();

  virtual void initializeInternalData(const std::vector<std::shared_ptr<Ray>> & particles) override;
  virtual void collideParticles(const std::vector<std::shared_ptr<Ray>> & particles) override;

protected:
  /// this will store the maximum value of the produce of
  /// the cross section and relative speed
  /// the first index will be the element and
  /// the second vector will store the value for each species pair
  std::vector<Real> _elem_volumes;
  std::vector<dof_id_type> _elem_ids;
  std::vector<std::vector<Real>> _elem_wise_max_cr_values;
};
