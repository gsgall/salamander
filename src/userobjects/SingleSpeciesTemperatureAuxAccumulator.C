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

#include "SingleSpeciesTemperatureAuxAccumulator.h"
#include "PICStudy.h"
#include "Constants.h"
#include "AuxAccumulator.h"

#include "AuxiliarySystem.h"
#include "libmesh/enum_fe_family.h"
#include <libmesh/enum_order.h>
#include <libmesh/fe_type.h>

registerMooseObject("SalamanderApp", SingleSpeciesTemperatureAuxAccumulator);

InputParameters
SingleSpeciesTemperatureAuxAccumulator::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the particles");
  params.addRequiredParam<AuxVariableName>(
      "aux_variable", "The name of the aux variable where we want to do store the data");
  params.addRequiredParam<std::string>(
      "species", "The name of the species of which you want to calculate the temperature.");
  return params;
}

SingleSpeciesTemperatureAuxAccumulator::SingleSpeciesTemperatureAuxAccumulator(
    const InputParameters & params)
  : GeneralUserObject(params),
    _study(getUserObject<PICStudy>("study")),
    _aux_variable(getParam<AuxVariableName>("aux_variable")),
    _species_id(_study.speciesId(getParam<std::string>("species")))
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
SingleSpeciesTemperatureAuxAccumulator::execute()
{
  auto accumulator = std::make_unique<SALAMANDER::AuxAccumulator>(_fe_problem, _aux_variable);

  const auto & particles = _study.particles();

  for (const auto & elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    const auto id = elem->id();
    Real mean_energy = 0;
    Real total_weight = 0.0;

    for (const auto particle : particles)
    {
      if (particle->currentElem()->id() != id || _study.species(*particle) != _species_id)
        continue;

      const auto weight = _study.weight(*particle);

      total_weight += weight;
      mean_energy += weight * _study.energy(*particle);
    }

    mean_energy /= total_weight;

    accumulator->add(
        *elem, elem->vertex_average(), mean_energy * 2.0 / (3.0 * Salamander::constants::k_b));
  }

  accumulator->finalize();
}
