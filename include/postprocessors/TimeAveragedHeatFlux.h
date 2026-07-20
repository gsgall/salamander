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

class BoundaryHeatFluxAccumulatorRayBC;

class TimeAveragedHeatFlux : public GeneralPostprocessor
{
public:
  static InputParameters validParams();

  TimeAveragedHeatFlux(const InputParameters & parameters);

  virtual void initialize() override {}
  virtual void execute() override;

  /**
   * This will return the current time step size.
   */
  virtual Real getValue() const override;

protected:
  const unsigned int _time_step_start;
  const BoundaryHeatFluxAccumulatorRayBC & _heat_flux_bc;

  Real _sum_flux;
  Real _sum_time;
};
