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

#include "TimeAveragedSingleSpeciesPeculiarTemperatureAuxAccumulator.h"
#include "PICStudy.h"
#include "Constants.h"
#include "AuxAccumulator.h"

#include "AuxiliarySystem.h"
#include "libmesh/enum_fe_family.h"
#include <libmesh/enum_order.h>
#include <libmesh/fe_type.h>

registerMooseObject("SalamanderApp", TimeAveragedSingleSpeciesPeculiarTemperatureAuxAccumulator);

InputParameters
TimeAveragedSingleSpeciesPeculiarTemperatureAuxAccumulator::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the particles");
  params.addRequiredParam<AuxVariableName>(
      "aux_variable", "The name of the aux variable where we want to do store the data");
  params.addRequiredParam<std::string>(
      "species", "The name of the species of which you want to calculate the temperature.");
  params.addParam<unsigned int>(
      "time_step_start", 1, "The time step after which you want to start time averaging");
  return params;
}

TimeAveragedSingleSpeciesPeculiarTemperatureAuxAccumulator::
    TimeAveragedSingleSpeciesPeculiarTemperatureAuxAccumulator(const InputParameters & params)
  : GeneralUserObject(params),
    _study(getUserObject<PICStudy>("study")),
    _aux_variable(getParam<AuxVariableName>("aux_variable")),
    _species_id(_study.speciesId(getParam<std::string>("species"))),
    _mass(_study.mass(_species_id)),
    _time_step_start(getParam<unsigned int>("time_step_start"))
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
  const auto num_local_elements = std::distance(elem_range.begin(), elem_range.end());

  _sum_velocity.resize(num_local_elements, Point(0.0, 0.0, 0.0));
  _sum_speed.resize(num_local_elements, 0.0);
}

void
TimeAveragedSingleSpeciesPeculiarTemperatureAuxAccumulator::execute()
{
  if (_t_step < _time_step_start)
    return;

  _sum_time += _dt;
  auto accumulator = std::make_unique<SALAMANDER::AuxAccumulator>(_fe_problem, _aux_variable);

  const auto & particles = _study.particles();

  size_t i = 0;
  for (const auto & elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    const auto current_elem_id = elem->id();
    Point local_sum_velocity = Point(0.0, 0.0, 0.0);
    Real local_sum_speed = 0.0;
    Real local_sum_weight = 0.0;

    for (const auto particle : particles)
    {
      if (particle->currentElem()->id() != current_elem_id ||
          _study.species(*particle) != _species_id)
        continue;

      _study.velocity(*particle, _temporary_velocity);
      const auto weight = _study.weight(*particle);

      local_sum_velocity += weight * _temporary_velocity;
      local_sum_speed += weight * _temporary_velocity.norm_sq();
      local_sum_weight += weight;
    }

    auto mean_speed = local_sum_speed / local_sum_weight;
    auto mean_velocity = local_sum_velocity / local_sum_weight;

    _sum_speed[i] += _dt * mean_speed;
    _sum_velocity[i] += _dt * mean_velocity;

    mean_speed = _sum_speed[i] / _sum_time;
    mean_velocity = _sum_velocity[i] / _sum_time;

    accumulator->add(*elem,
                     elem->vertex_average(),
                     _mass * (mean_speed - mean_velocity.norm_sq()) /
                         (3.0 * Salamander::constants::k_b));
    ++i;
  }

  accumulator->finalize();
}
