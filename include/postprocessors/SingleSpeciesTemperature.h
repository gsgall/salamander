//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralPostprocessor.h"
#include "Ray.h"

class PICStudyBase;
class SingleSpeciesTemperature : public GeneralPostprocessor
{
public:
  static InputParameters validParams();

  SingleSpeciesTemperature(const InputParameters & parameters);

  virtual void initialSetup() override;
  virtual void initialize() override {}
  virtual void execute() override;
  virtual void finalize() override;
  virtual Real getValue() const override;

protected:
  /// the study that we are collecting data from
  const std::string & _study_name;
  const unsigned int _species_id;
  const PICStudyBase * _study;
  std::vector<RayDataIndex> _velocity_indicies;
  RayDataIndex _species_idx;
  RayDataIndex _mass_idx;
  Real _temperature;
  size_t _particle_count;
  Real _energy_sum;
};
