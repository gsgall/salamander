//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SingleSpeciesTotalEnergy.h"

#include "PICStudyBase.h"

registerMooseObject("SalamanderApp", SingleSpeciesTotalEnergy);

InputParameters
SingleSpeciesTotalEnergy::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the particles");
  params.addRequiredParam<std::string>(
      "species", "The name of the species of which you want to calculate the temperature.");
  return params;
}

SingleSpeciesTotalEnergy::SingleSpeciesTotalEnergy(const InputParameters & params)
  : GeneralPostprocessor(params), _study(getUserObject<PICStudyBase>("study"))
{
}

void
SingleSpeciesTotalEnergy::initialSetup()
{
  _mass = _study.mass(getParam<std::string>("species"));
}

void
SingleSpeciesTotalEnergy::execute()
{
  const auto & particles = _study.bankedParticles();
  _total_energy = 0.0;
  for (const auto & particle : particles)
  {
    _total_energy = 0.5 * _mass * _study.weight(*particle) * _study.velocity(*particle).norm_sq();
  }
}

void
SingleSpeciesTotalEnergy::finalize()
{
  comm().sum(_total_energy);
}

Real
SingleSpeciesTotalEnergy::getValue() const
{
  return _total_energy;
}
