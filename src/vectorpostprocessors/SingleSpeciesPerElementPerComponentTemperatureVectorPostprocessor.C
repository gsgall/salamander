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

#include "SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor.h"
#include "Constants.h"
#include "PICStudyBase.h"
#include <unordered_map>

registerMooseObject("SalamanderApp",
                    SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor);

constexpr Real k_B = 1.380649e-23;

InputParameters
SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();
  params.addClassDescription(
      "Collects data which is stored in RayData on particles on a per timestep basis.");
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the Ray");
  params.addRequiredParam<std::string>(
      "species", "The name of the species of which you want to calculate the temperature.");
  params.addRequiredRangeCheckedParam<unsigned int>(
      "component", "component < 3", "The component for which you are taking the temperature");
  return params;
}

SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor::
    SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor(
        const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _study(getUserObject<PICStudyBase>("study")),
    _species_id(_study.speciesId(getParam<std::string>("species"))),
    _species_index(_study.speciesIndex()),
    _mass_index(_study.getRayDataIndex("mass")),
    _velocity_index(_study.velocityIndicies()[getParam<unsigned int>("component")])
{
  const auto & elem_range = *_fe_problem.mesh().getActiveLocalElementRange();
  _num_elems = std::distance(elem_range.begin(), elem_range.end());
  comm().sum(_num_elems);

  _local_values.resize(2 * _num_elems);
  for (size_t i = 0; i < _num_elems; i++)
  {
    _data_values.push_back(&declareVector("T_" + std::to_string(i)));
    _data_values.push_back(&declareVector("N_p_" + std::to_string(i)));
  }
  _data_values.push_back(&declareVector("t_vel"));
}

void
SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor::initialize()
{
  for (size_t i = 0; i < 2 * _num_elems; i++)
  {
    _local_values[i] = 0;
  }
}

void
SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor::execute()
{

  // key is element id and count is particle in element
  std::unordered_map<unsigned int, unsigned int> particle_count;
  std::unordered_map<unsigned int, Real> element_temperatures;
  for (size_t i = 0; i < _num_elems; i++)
  {
    particle_count[i] = 0;
    element_temperatures[i] = 0;
  }

  for (const auto & ray : _study.bankedParticles())
  {
    if (ray->data(_species_index) != _species_id)
      continue;

    unsigned int elem_id = ray->currentElem()->id();

    Real local_sum = 0;
    local_sum += ray->data(_mass_index) * ray->data(_velocity_index) * ray->data(_velocity_index);

    particle_count[elem_id] += 1;
    element_temperatures[elem_id] += local_sum / Salamander::constants::k_b;
  }

  for (size_t i = 0; i < _num_elems; i++)
  {
    if (particle_count[i] == 0)
      continue;
    _local_values[2 * i] = (element_temperatures[i] / particle_count[i]);
    _local_values[2 * i + 1] = (particle_count[i]);
  }
}

void
SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor::finalize()
{
  for (size_t i = 0; i < _num_elems; i++)
  {
    Real temp = _local_values[2 * i];
    comm().sum(temp);
    _local_values[2 * i] = temp;
    temp = _local_values[2 * i + 1];
    comm().sum(temp);
    _local_values[2 * i + 1] = temp;
  }

  if (comm().rank() != 0)
    return;

  for (const auto i : make_range(_data_values.size() - 1))
    _data_values[i]->push_back(_local_values[i]);

  _data_values.back()->push_back(_t - _dt / 2);
}
