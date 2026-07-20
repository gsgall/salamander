//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "VariableTimeAverageAux.h"

registerMooseObject("MooseApp", VariableTimeAverageAux);

InputParameters
VariableTimeAverageAux::validParams()
{
  InputParameters params = VariableTimeIntegrationAux::validParams();
  params.addClassDescription("Integrates a field variable in time.");
  params.addParam<unsigned int>(
      "time_step_start", 1, "The timesptep on which time averaging will begin");

  params.setParameters<unsigned int>("order", 1);
  params.suppressParameter<unsigned int>("order");

  return params;
}

VariableTimeAverageAux::VariableTimeAverageAux(const InputParameters & parameters)
  : VariableTimeIntegrationAux(parameters),
    _time_step_start(getParam<unsigned int>("time_step_start"))
{
  if (_order > 2)
    paramError("order",
               "Greater than second order time integration is not currently supported for time "
               "averaging.");
}

void
VariableTimeAverageAux::timestepSetup()
{
  if (_t_step < _time_step_start)
    return;

  _sum_time += _dt;
}

Real
VariableTimeAverageAux::computeValue()
{
  if (_t_step < _time_step_start)
    return 0.0;
  std::cerr << _t_step << std::endl;
  std::cerr << _time_step_start << std::endl;
  std::cerr << _u_old[_qp] << std::endl;
  Real integral = getIntegralValue();
  return (_u_old[_qp] * (_sum_time - _dt) + _coef * integral) / _sum_time;
}
