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
  params.addRequiredParam<std::vector<std::string>>(
      "reactants", "The species on the left hand side of the reaction");
  params.addRequiredParam<std::vector<std::string>>(
      "products", "The species on the right hand side of the reaction");
  params.addParam<unsigned int>("seed", 0, "The seed value for the random number generator");
  return params;
}

CollisionBase::CollisionBase(const InputParameters & parameters) : GeneralUserObject(parameters)
{
  _generator.seed(getParam<unsigned int>("seed"));
}

void
CollisionBase::initialSetup()
{
  _study = &getUserObject<CollisionalPICStudy>("study");
  _species_index = _study->speciesIndex();
  _weight_index = _study->weightIndex();
  _mass_index = _study->massIndex();

  const auto reactant_names = getParam<std::vector<std::string>>("reactants");
  const auto product_names = getParam<std::vector<std::string>>("products");

  // both of these loops might error since the call of
  // _study.speciesId(name) calls moosError in the case
  // that a provided species name is not known to the study
  for (const auto & name : reactant_names)
  {
    _reactant_ids.push_back(_study->speciesId(name));
  }

  for (const auto & name : product_names)
  {
    _product_ids.push_back(_study->speciesId(name));
  }
}

const Real
CollisionBase::relativeSpeed(const Ray & particle_a, const Ray & particle_b) const
{
  Point vel_a, vel_b;
  _study->getVelocityData(particle_a, vel_a);
  _study->getVelocityData(particle_b, vel_b);
  return (vel_a - vel_b).norm();
}
const Real
CollisionBase::centerOfMassEnergy(const Ray & particle_a, const Ray & particle_b) const
{
  const auto m_a = particle_a.data(_mass_index);
  const auto m_b = particle_b.data(_mass_index);
  const auto reduced_mass = m_a * m_b / (m_a + m_b);

  Point vel_a, vel_b;
  _study->getVelocityData(particle_a, vel_a);
  _study->getVelocityData(particle_b, vel_b);

  return 0.5 * reduced_mass * (vel_a - vel_b).norm_sq();
}

const Point
CollisionBase::centerOfMassVelocity(const Ray & particle_a, const Ray & particle_b) const
{
  const auto m_a = particle_a.data(_mass_index);
  const auto m_b = particle_b.data(_mass_index);

  Point vel_a, vel_b;
  _study->getVelocityData(particle_a, vel_a);
  _study->getVelocityData(particle_b, vel_b);

  return (m_a * vel_a + m_b * vel_b) / (m_a + m_b);
}

const std::vector<unsigned int> &
CollisionBase::reactantIds() const
{
  return _reactant_ids;
}

const std::vector<unsigned int> &
CollisionBase::productIds() const
{
  return _product_ids;
}
