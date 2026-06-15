//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "Constants.h"
#include "SingleSpeciesTemperature.h"

#include "PICStudyBase.h"

registerMooseObject("SalamanderApp", SingleSpeciesTemperature);

InputParameters
SingleSpeciesTemperature::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the particles");
  params.addRequiredParam<std::string>(
      "species", "The name of the species of which you want to calculate the temperature.");
  params.addRequiredRangeCheckedParam<unsigned int>(
      "component", "component < 3", "The component for the temperautre");
  return params;
}

SingleSpeciesTemperature::SingleSpeciesTemperature(const InputParameters & params)
  : GeneralPostprocessor(params),
    _study(getUserObject<PICStudyBase>("study")),
    _component(getParam<unsigned int>("component"))
{
}

void
SingleSpeciesTemperature::initialSetup()
{
  _mass = _study.mass(getParam<std::string>("species"));
}

void
SingleSpeciesTemperature::execute()
{
  const auto & particles = _study.bankedParticles();
  _total_energy = 0.0;
  _total_weight = 0.0;
  for (const auto & particle : particles)
  {
    _total_weight += _study.weight(*particle);
    _total_energy += _mass * _study.weight(*particle) * _study.velocity(*particle)(_component) *
                     _study.velocity(*particle)(_component);
  }
}

void
SingleSpeciesTemperature::finalize()
{
  comm().sum(_total_weight);
  comm().sum(_total_energy);
}

Real
SingleSpeciesTemperature::getValue() const
{
  return _total_energy / (_total_weight * Salamander::constants::k_b);
}
