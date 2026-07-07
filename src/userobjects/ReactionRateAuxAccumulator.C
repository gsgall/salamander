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
//* Copyright 2025, Battelle Energy Alliance, LLC
//* ALL RIGHTS RESERVED
//*

#include "ReactionRateAuxAccumulator.h"
#include "PICStudy.h"
#include "AuxAccumulator.h"

#include "AuxiliarySystem.h"
#include "libmesh/enum_fe_family.h"
#include <libmesh/enum_order.h>
#include <libmesh/fe_type.h>

registerMooseObject("SalamanderApp", ReactionRateAuxAccumulator);

InputParameters
ReactionRateAuxAccumulator::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy being used");
  params.addRequiredParam<AuxVariableName>(
      "aux_variable", "The name of the aux variable where we want to do store the data");
  params.addRequiredParam<UserObjectName>(
      "collision", "The name of the collision object of which you want to querey the rates.");
  params.addRequiredParam<UserObjectName>("collider",
                                          "The collider which owns the collision requested");
  return params;
}

ReactionRateAuxAccumulator::ReactionRateAuxAccumulator(const InputParameters & params)
  : GeneralUserObject(params),
    _study(getUserObject<PICStudy>("study")),
    _aux_variable(getParam<AuxVariableName>("aux_variable")),
    _collider(getUserObject<ParticleColliderBase>("collider"))
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
}

void
ReactionRateAuxAccumulator::initialSetup()
{
  _indicies = _collider.collisionIndices(getParam<UserObjectName>("collision"));
}

void
ReactionRateAuxAccumulator::execute()
{
  auto accumulator = std::make_unique<SALAMANDER::AuxAccumulator>(_fe_problem, _aux_variable);

  _collider.fillAuxAccumulator(_indicies, *accumulator);

  accumulator->finalize();
}
