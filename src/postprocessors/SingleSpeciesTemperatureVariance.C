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
#include "SingleSpeciesTemperatureVariance.h"
registerMooseObject("SalamanderApp", SingleSpeciesTemperatureVariance);

InputParameters
SingleSpeciesTemperatureVariance::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the particles");
  params.addRequiredParam<Real>("temperature", "The temperature that the species should be at");
  params.addRequiredParam<unsigned int>("species_id",
                                        "The species id for the species we are interested in");
  return params;
}

SingleSpeciesTemperatureVariance::SingleSpeciesTemperatureVariance(const InputParameters & params)
  : GeneralPostprocessor(params),
    _study_name(getParam<UserObjectName>("study")),
    _known_T(getParam<Real>("temperature")),
    _species_id(getParam<unsigned int>("species_id"))
{
}

void
SingleSpeciesTemperatureVariance::initialSetup()
{
  _study = &getUserObjectByName<PICStudyBase>(_study_name);
  _velocity_indicies = _study->getVelocityIndicies(true);
  _species_idx = _study->getRayDataIndex("species");
  _mass_idx = _study->getRayDataIndex("mass");
}

void
SingleSpeciesTemperatureVariance::execute()
{
  _energy_sum = 0;
  _particle_count = 0;
  for (const auto & particle : _study->getBankedRays())
  {
    if (particle->data(_species_idx) != _species_id)
      continue;

    Real local_sum = 0;
    for (const auto idx : _velocity_indicies)
    {
      local_sum += particle->data(idx) * particle->data(idx);
    }
    std::cout << particle->data(_mass_idx) / (3.0 * Salamander::constants::k_b) * local_sum
              << std::endl;
    Real val =
        (particle->data(_mass_idx) / (3.0 * Salamander::constants::k_b) * local_sum - _known_T);
    _energy_sum += val * val;
    _particle_count++;
  }
  comm().sum(_energy_sum);
  comm().sum(_particle_count);
  _temperature = _energy_sum / _particle_count;
}

void
SingleSpeciesTemperatureVariance::finalize()
{
}

Real
SingleSpeciesTemperatureVariance::getValue() const
{
  return _temperature;
}
