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
  const PICStudyBase & _study;
  const unsigned int _component;
  Real _mass;
  Real _total_energy;
  Real _total_weight;
};
