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

#include "SingleSpeciesPerElementTemperatureVectorPostprocessor.h"
#include "PICStudyBase.h"
#include <unordered_map>

registerMooseObject("SalamanderApp", SingleSpeciesPerElementTemperatureVectorPostprocessor);

constexpr Real k_B = 1.380649e-23;
InputParameters
SingleSpeciesPerElementTemperatureVectorPostprocessor::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();
  params.addClassDescription(
      "Collects data which is stored in RayData on particles on a per timestep basis.");
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the Ray");
  params.addRequiredParam<std::string>(
      "species", "The name of the species of which you want to calculate the temperature.");
  params.addRequiredParam<unsigned int>("num_elems", "The number of elements in the domain");
  return params;
}

SingleSpeciesPerElementTemperatureVectorPostprocessor::
    SingleSpeciesPerElementTemperatureVectorPostprocessor(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _study(getUserObject<PICStudyBase>("study")),
    _species_index(_study.getRayDataIndex("species")),
    _mass_index(_study.getRayDataIndex("mass")),
    _velocity_indicies({
        _study.getRayDataIndex("v_x"),
        _study.getRayDataIndex("v_y"),
        _study.getRayDataIndex("v_z"),
    }),
    _species_id(_study.speciesId(getParam<std::string>("species"))),
    _num_elems(getParam<unsigned int>("num_elems"))
{
  _local_values.resize(2 * _num_elems);
  for (size_t i = 0; i < _num_elems; i++)
  {
    _data_values.push_back(&declareVector("T_" + std::to_string(i)));
    _data_values.push_back(&declareVector("N_p_" + std::to_string(i)));
  }
  _data_values.push_back(&declareVector("t_vel"));
}

void
SingleSpeciesPerElementTemperatureVectorPostprocessor::initialize()
{
  for (size_t i = 0; i < 2 * _num_elems; i++)
  {
    _local_values[i] = 0;
  }
}

void
SingleSpeciesPerElementTemperatureVectorPostprocessor::execute()
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
    for (const auto idx : _velocity_indicies)
    {
      local_sum += ray->data(_mass_index) * ray->data(idx) * ray->data(idx);
    }

    particle_count[elem_id] += 1;
    element_temperatures[elem_id] += local_sum / (3.0 * k_B);
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
SingleSpeciesPerElementTemperatureVectorPostprocessor::finalize()
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
