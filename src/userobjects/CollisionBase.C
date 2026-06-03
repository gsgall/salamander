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

#include "CollisionBase.h"
#include "CollisionalPICStudy.h"
#include "InputParameters.h"

InputParameters
CollisionBase::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addClassDescription(
      "Base class for the actual collision logic for any given type of collision.");
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the Ray");
  return params;
}

CollisionBase::CollisionBase(const InputParameters & parameters)
  : GeneralUserObject(parameters), _study(getUserObject<PICStudyBase>("study"))
{
}
