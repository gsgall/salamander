//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic
// confinement for Numerical Design, Engineering & Research,
//* A multiphysics application for modeling plasma facing components
//* https://github.com/idaholab/salamander
//* https://mooseframework.inl.gov/salamander
//*
//* SALAMANDER is powered by the MOOSE Framework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
//* Copyright 2025, Battelle Energy Alliance, LLC and North Carolina State University
//* ALL RIGHTS RESERVED
//*
#include "AuxiliarySystem.h"
#include "BoundaryHeatFluxAccumulatorRayBC.h"
#include "NonZeroedAuxAccumulator.h"
#include "PICStudy.h"
#include "RayBoundaryConditionBase.h"

registerMooseObject("SalamanderApp", BoundaryHeatFluxAccumulatorRayBC);

InputParameters
BoundaryHeatFluxAccumulatorRayBC::validParams()
{
  auto params = RayBoundaryConditionBase::validParams();
  params.addClassDescription(
      "Reflective boundary condition that resamples the particles velocity when it hits the "
      "boundary."
      "This condition requires that each component of velocity is sampled it's own distribution."
      "Currently this condition only works for x component.");
  params.addRequiredParam<AuxVariableName>(
      "aux_variable", "The name of the aux variable where we want to do store the data");

  params.addParam<unsigned int>("time_step_start",
                                "The time step after which we are going to start time averaging.");

  return params;
}

BoundaryHeatFluxAccumulatorRayBC::BoundaryHeatFluxAccumulatorRayBC(const InputParameters & params)
  : RayBoundaryConditionBase(params),
    _pic_study(getStudy<PICStudy>()),
    _aux_variable(getParam<AuxVariableName>("aux_variable")),
    _time_step_start(getParam<unsigned int>("time_step_start")),
    _mesh_dimension(_fe_problem.mesh().dimension())
{
  auto & aux = _fe_problem.getAuxiliarySystem();
  auto & var = aux.getFieldVariable<Real>(0, getParam<AuxVariableName>("aux_variable"));
  auto & type = var.feType();
  if (type.family != libMesh::MONOMIAL || type.order != libMesh::CONSTANT)
  {
    paramError("aux_variable",
               "This accumulator currently only supports aux variables of with order = CONSTANT "
               "and family = MONOMIAL");
  }
  _sum_time = 0.0;
  _temporary_velocity = Point(0.0, 0.0, 0.0);
}

void
BoundaryHeatFluxAccumulatorRayBC::timestepSetup()
{
  if (_t_step < _time_step_start)
    return;

  accumulator = std::make_unique<SALAMANDER::NonZeroedAuxAccumulator>(_fe_problem, _aux_variable);
  accumulator->scale(_sum_time);
  _sum_time += _dt;
}

void
BoundaryHeatFluxAccumulatorRayBC::preOnBoundary(const unsigned int num_applying)
{

  if (_t_step < _time_step_start)
    return;

  const auto & particle = currentRay();
  const auto area = _current_elem->build_side_ptr(_current_intersected_side)->volume();

  accumulator->add(*_current_elem,
                   _current_elem->vertex_average(),
                   _pic_study.weight(*particle) * _pic_study.energy(*particle) / area);
}

void
BoundaryHeatFluxAccumulatorRayBC::postOnBoundary(const unsigned int num_applying)
{
  if (_t_step < _time_step_start)
    return;

  const auto & particle = currentRay();
  const auto area = _current_elem->build_side_ptr(_current_intersected_side)->volume();

  accumulator->add(*_current_elem,
                   _current_elem->vertex_average(),
                   -_pic_study.weight(*particle) * _pic_study.energy(*particle) / area);
}

void
BoundaryHeatFluxAccumulatorRayBC::postExecuteStudy()
{
  if (_t_step < _time_step_start)
    return;
  accumulator->scale(1.0 / _sum_time);
  accumulator->finalize();
}
