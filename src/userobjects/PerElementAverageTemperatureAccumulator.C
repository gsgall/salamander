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

registerMooseObject("SalamanderApp", PerElementAverageTemperatureAccumulator);

constexpr Real k_B = 1.380649e-23;

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

  const auto & elem_range = *_fe_problem.mesh().getActiveLocalElementRange();
  const auto num_local_elements = std::distance(elem_range.begin(), elem_range.end());

  _total_energy_per_cell.resize(num_local_elements);
  _total_weights_per_cell.resize(num_local_elements);

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

  auto accumulator = std::make_unique<SALAMANDER::AuxAccumulator>(
      _fe_problem, getParam<AuxVariableName>("aux_variable"));
  const auto particles = _study.bankedParticles();
  size_t i = 0;
  for (const auto & elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    _total_energy_per_cell[i] = 0.0;
    _total_weights_per_cell[i] = 0.0;
    const auto id = elem->id();

    for (const auto particle : particles)
    {
      const auto mass = particle->data(_mass_index);
      const auto weight = particle->data(_weight_index);

      if (particle->currentElem()->id() != id || particle->data(_species_index) != _species_id)
        continue;

      _total_weights_per_cell[i] += weight;

      RayData speed_squared = 0.0;
      for (const auto j : _velocity_indicies)
      {
        speed_squared += particle->data(j) * particle->data(j);
      }
      _total_energy_per_cell[i] += weight * mass * speed_squared;
    }
    ++i;
  }

  i = 0;
  for (const auto & elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    accumulator->add(*elem,
                     elem->vertex_average(),
                     _total_energy_per_cell[i] / (3.0 * k_B * _total_weights_per_cell[i]));
    ++i;
  }
  accumulator->finalize();
}
