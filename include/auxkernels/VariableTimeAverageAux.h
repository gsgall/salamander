//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "VariableTimeIntegrationAux.h"

/**
 * An AuxKernel that can be used to integrate a field variable in time
 * using a variety of different integration methods.  The result is
 * stored in another field variable.
 */
class VariableTimeAverageAux : public VariableTimeIntegrationAux
{
public:
  static InputParameters validParams();

  VariableTimeAverageAux(const InputParameters & parameters);

  virtual void timestepSetup() override;

protected:
  virtual Real computeValue() override;

  const unsigned int _time_step_start;
  Real _sum_time;
};
