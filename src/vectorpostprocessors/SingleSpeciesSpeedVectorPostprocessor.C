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

#include "SingleSpeciesSpeedVectorPostprocessor.h"
#include "PICStudyBase.h"

registerMooseObject("SalamanderApp", SingleSpeciesSpeedVectorPostprocessor);

InputParameters
SingleSpeciesSpeedVectorPostprocessor::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();
  params.addClassDescription(
      "Collects data which is stored in RayData on particles on a per timestep basis.");
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the Ray");
  params.addRequiredParam<unsigned int>("species_id",
                                        "The species id for the species we are interested in");
  return params;
}

SingleSpeciesSpeedVectorPostprocessor::SingleSpeciesSpeedVectorPostprocessor(
    const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _study(getUserObject<PICStudyBase>("study")),
    _species_index(_study.getRayDataIndex("species")),
    _v_x_index(_study.getRayDataIndex("v_x")),
    _v_y_index(_study.getRayDataIndex("v_y")),
    _v_z_index(_study.getRayDataIndex("v_z")),
    _species_id(getParam<unsigned int>("species_id")),
    _speeds(&declareVector("speed"))
{
}

void
SingleSpeciesSpeedVectorPostprocessor::initialize()
{
  _speeds->clear();
}

void
SingleSpeciesSpeedVectorPostprocessor::execute()
{
  for (const auto & ray : _study.getBankedRays())
  {
    if (ray->data(_species_index) != _species_id)
      continue;

    _speeds->push_back(std::sqrt(ray->data(_v_x_index) * ray->data(_v_x_index) +
                                 ray->data(_v_y_index) * ray->data(_v_y_index) +
                                 ray->data(_v_z_index) * ray->data(_v_z_index)));
  }
}

void
SingleSpeciesSpeedVectorPostprocessor::finalize()
{
  comm().gather(0, *_speeds);
}
