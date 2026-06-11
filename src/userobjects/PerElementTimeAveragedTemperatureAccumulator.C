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

#include "PerElementTimeAveragedTemperatureAccumulator.h"
#include "PICStudyBase.h"
#include "AuxAccumulator.h"

#include "AuxiliarySystem.h"
#include "MooseMesh.h"
#include "libmesh/enum_fe_family.h"
#include "libmesh/enum_elem_type.h"
#include <libmesh/enum_order.h>
#include <libmesh/fe_type.h>

#include "Constants.h"

registerMooseObject("SalamanderApp", PerElementTimeAveragedTemperatureAccumulator);

InputParameters
PerElementTimeAveragedTemperatureAccumulator::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the particles");
  params.addRequiredParam<AuxVariableName>(
      "aux_variable", "The name of the aux variable where we want to do store the data");
  params.addRequiredParam<std::string>(
      "species", "The name of the species of which you want to calculate the temperature.");
  params.addRequiredParam<unsigned int>(
      "start_averaging_step", "the time step after which you would like to start averaging.");
  return params;
}

PerElementTimeAveragedTemperatureAccumulator::PerElementTimeAveragedTemperatureAccumulator(
    const InputParameters & params)
  : GeneralUserObject(params),
    _study(getUserObject<PICStudyBase>("study")),
    _species_id(_study.speciesId(getParam<std::string>("species"))),
    _time_step_start(getParam<unsigned int>("start_averaging_step"))
{

  auto & aux = _fe_problem.getAuxiliarySystem();
  auto & var = aux.getFieldVariable<Real>(0, getParam<AuxVariableName>("aux_variable"));
  auto & type = var.feType();
  if (type.family != libMesh::MONOMIAL || type.order != libMesh::CONSTANT)
  {
    paramError("aux_variable",
               "This accumulator currently only supports aux variables of with order = CONSTANT "
               "and family = MONOMIAL");
  }

  const auto & elem_range = *_fe_problem.mesh().getActiveLocalElementRange();
  const auto elem_count = std::distance(elem_range.begin(), elem_range.end());

  _total_weight = std::vector<Real>(elem_count, 0.0);
  _total_velocity = std::vector<Point>(elem_count, Point(0, 0, 0));
  _total_speed = std::vector<Real>(elem_count, 0.0);
}

void
PerElementTimeAveragedTemperatureAccumulator::execute()
{
  const auto mass = _study.mass(_species_id);
  auto accumulator = std::make_unique<SALAMANDER::AuxAccumulator>(
      _fe_problem, getParam<AuxVariableName>("aux_variable"));
  const auto particles = _study.bankedParticles();

  size_t i = 0;
  for (const auto & elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    auto & total_weight = _total_weight[i];
    auto & total_velocity = _total_velocity[i];
    auto & total_speed = _total_speed[i];

    const auto id = elem->id();
    for (const auto particle : particles)
    {
      if (particle->currentElem()->id() != id || _study.species(*particle) != _species_id)
        continue;

      const auto weight = _study.weight(*particle);
      const auto velocity = _study.velocity(*particle);

      total_weight += weight;
      total_velocity += weight * velocity;
      total_speed += weight * mass * velocity.norm_sq();
    }

    const auto mean_velocity = total_velocity / total_weight;
    const auto mean_speed = total_speed / total_weight;
    const auto temperature =
        (mean_speed - mass * mean_velocity.norm_sq()) / (3 * Salamander::constants::k_b);

    accumulator->add(*elem, elem->vertex_average(), temperature);
    ++i;
  }
  accumulator->finalize();
}
