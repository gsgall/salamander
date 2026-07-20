//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "BoundaryHeatFluxAccumulatorRayBC.h"
#include "TimeAveragedHeatFlux.h"

registerMooseObject("MooseApp", TimeAveragedHeatFlux);

InputParameters
TimeAveragedHeatFlux::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addClassDescription("");
  params.addRequiredParam<UserObjectName>(
      "heat_flux_bc",
      "The ray boundary condition that is storing and accumulating the wall heat flux");
  params.addParam<unsigned int>(
      "time_step_start", 1, "The time step after which you want to start time averaging");

  return params;
}

TimeAveragedHeatFlux::TimeAveragedHeatFlux(const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    _time_step_start(getParam<unsigned int>("time_step_start")),
    _heat_flux_bc(getUserObject<BoundaryHeatFluxAccumulatorRayBC>("heat_flux_bc"))
{
}

void
TimeAveragedHeatFlux::execute()
{
  if (_t_step < _time_step_start)
    return;

  _sum_time += _dt;
  _sum_flux += _dt * _heat_flux_bc.heatFlux();
}

Real
TimeAveragedHeatFlux::getValue() const
{
  if (_t_step < _time_step_start)
    return 0;

  return _sum_flux / _sum_time;
}
