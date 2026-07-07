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
#include "PICStudy.h"
#include "InputParameters.h"

InputParameters
CollisionBase::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addClassDescription(
      "Base class for the actual collision logic for any given type of collision.");
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the Ray");
  params.addRequiredParam<std::vector<std::string>>(
      "reactants", "The species on the left hand side of the reaction");
  params.addRequiredParam<std::vector<std::string>>(
      "products", "The species on the right hand side of the reaction");
  params.addParam<unsigned int>("seed", 0, "The seed value for the random number generator");
  return params;
}

CollisionBase::CollisionBase(const InputParameters & parameters) : GeneralUserObject(parameters)
{
  const auto & reactants = getParam<std::vector<std::string>>("reactants");
  const auto & products = getParam<std::vector<std::string>>("products");

  if (reactants.size() != 2)
  {
    paramError("reactants", "You must supply two species for the reactants.");
  }

  if (products.size() != 2)
  {
    paramError("products", "You must supply two species for the products.");
  }

  _generator.seed(getParam<unsigned int>("seed"));
}

void
CollisionBase::initialSetup()
{
  _study = &getUserObject<PICStudy>("study");

  const auto reactant_names = getParam<std::vector<std::string>>("reactants");
  const auto product_names = getParam<std::vector<std::string>>("products");

  // both of these loops might error since the call of
  // _study.speciesId(name) calls moosError in the case
  // that a provided species name is not known to the study
  for (size_t i = 0; i < _reactant_ids.size(); ++i)
  {
    _reactant_ids[i] = _study->speciesId(reactant_names[i]);
    _product_ids[i] = _study->speciesId(product_names[i]);
  }
}

const std::array<unsigned int, 2> &
CollisionBase::reactantIds() const
{
  return _reactant_ids;
}

const std::array<unsigned int, 2> &
CollisionBase::productIds() const
{
  return _product_ids;
}
