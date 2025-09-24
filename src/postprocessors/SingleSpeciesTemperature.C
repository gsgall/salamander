//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PICStudyBase.h"
#include "Constants.h"
#include "SingleSpeciesTemperature.h"
registerMooseObject("SalamanderApp", SingleSpeciesTemperature);

InputParameters
SingleSpeciesTemperature::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the particles");
  params.addRequiredParam<unsigned int>("species_id",
                                        "The species id for the species we are interested in");
  return params;
}

SingleSpeciesTemperature::SingleSpeciesTemperature(const InputParameters & params)
  : GeneralPostprocessor(params),
    _study_name(getParam<UserObjectName>("study")),
    _species_id(getParam<unsigned int>("species_id"))
{
}

void
SingleSpeciesTemperature::initialSetup()
{
  _study = &getUserObjectByName<PICStudyBase>(_study_name);
  _velocity_indicies = _study->getVelocityIndicies(true);
  _species_idx = _study->getRayDataIndex("species");
  _mass_idx = _study->getRayDataIndex("mass");
}

void
SingleSpeciesTemperature::execute()
{
  _energy_sum = 0;
  _particle_count = 0;
  for (const auto & particle : _study->getBankedRays())
  {
    if (particle->data(_species_idx) != _species_id)
      continue;

    Real local_sum = 0;
    for (const auto idx : _velocity_indicies)
      local_sum += particle->data(idx) * particle->data(idx);
    _energy_sum += 0.5 * particle->data(_mass_idx) * local_sum;
    _particle_count++;
  }
  comm().sum(_energy_sum);
  comm().sum(_particle_count);
  _temperature = (2.0 / 3.0) * _energy_sum / (_particle_count * Salamander::constants::k_b);
}

void
SingleSpeciesTemperature::finalize()
{
}

Real
SingleSpeciesTemperature::getValue() const
{
  return _temperature;
}
