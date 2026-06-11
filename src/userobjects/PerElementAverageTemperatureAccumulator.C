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

#include "PerElementAverageTemperatureAccumulator.h"
#include "PICStudyBase.h"
#include "AuxAccumulator.h"

#include "AuxiliarySystem.h"
#include "MooseMesh.h"
#include "libmesh/enum_fe_family.h"
#include "libmesh/enum_elem_type.h"
#include <libmesh/enum_order.h>
#include <libmesh/fe_type.h>

#include "Constants.h"

registerMooseObject("SalamanderApp", PerElementAverageTemperatureAccumulator);

InputParameters
PerElementAverageTemperatureAccumulator::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the particles");
  params.addRequiredParam<AuxVariableName>(
      "aux_variable", "The name of the aux variable where we want to do store the data");
  params.addRequiredParam<std::string>(
      "species", "The name of the species of which you want to calculate the temperature.");
  return params;
}

PerElementAverageTemperatureAccumulator::PerElementAverageTemperatureAccumulator(
    const InputParameters & params)
  : GeneralUserObject(params),
    _study(getUserObject<PICStudyBase>("study")),
    _species_id(_study.speciesId(getParam<std::string>("species"))),
    _velocity_indicies(_study.velocityIndicies()),
    _species_index(_study.speciesIndex()),
    _mass_index(_study.massIndex()),
    _weight_index(_study.weightIndex())
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
}

void
PerElementAverageTemperatureAccumulator::execute()
{
  //
  //  auto accumulator = std::make_unique<SALAMANDER::AuxAccumulator>(
  //      _fe_problem, getParam<AuxVariableName>("aux_variable"));
  //  const auto particles = _study.bankedParticles();
  //  for (const auto & elem : *_fe_problem.mesh().getActiveLocalElementRange())
  //  {
  //    const auto id = elem->id();
  //    Point mean_velocity = Point(0, 0, 0);
  //    Real total_weight = 0.0;
  //
  //    for (const auto particle : particles)
  //    {
  //      if (particle->currentElem()->id() != id || _study.species(*particle) != _species_id)
  //        continue;
  //
  //      const auto weight = _study.weight(*particle);
  //
  //      mean_velocity += weight * _study.velocity(*particle);
  //      total_weight += weight;
  //    }
  //
  //    mean_velocity /= total_weight;
  //    Real total_difference = 0.0;
  //
  //    for (const auto particle : particles)
  //    {
  //      if (particle->currentElem()->id() != id || _study.species(*particle) != _species_id)
  //        continue;
  //
  //      const auto weight = _study.weight(*particle);
  //      const auto difference = (_study.velocity(*particle) - mean_velocity);
  //      total_difference += difference;
  //    }
  //  }
  //
  //  for (const auto & elem : *_fe_problem.mesh().getActiveLocalElementRange())
  //  {
  //    accumulator->add(*elem,
  //                     elem->vertex_average(),
  //                     _total_energy_per_cell[i] /
  //                         (3.0 * Salamander::constants::k_b * _total_weights_per_cell[i]));
  //    ++i;
  //  }
  //  accumulator->finalize();
}
