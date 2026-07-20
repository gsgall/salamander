//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic
//* confinement for Numerical Design, Engineering & Research,
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

#include "NonZeroedAuxAccumulator.h"

#include "FEProblemBase.h"
#include "AuxiliarySystem.h"
#include "MooseVariableFE.h"
#include "MooseMesh.h"

namespace SALAMANDER
{
NonZeroedAuxAccumulator::NonZeroedAuxAccumulator(FEProblemBase & problem,
                                                 const AuxVariableName & variable)
  : AccumulatorBase(problem),
    _dim(_problem.mesh().dimension()),
    _aux(_problem.getAuxiliarySystem()),
    _var(_aux.getFieldVariable<Real>(0, variable)),
    _fe(FEGenericBase<Real>::build(_dim, _var.feType())),
    _fe_map(FEMap::build(_var.feType()))
{
}

void
NonZeroedAuxAccumulator::scale(const Real scale_factor)
{
  _aux.solution().scale(scale_factor);
}

void
NonZeroedAuxAccumulator::add(const Elem & elem, const Point & point, const Real & value)
{
  _fe->request_phi();
  const std::vector<Point> master_points = {_fe_map->inverse_map(_dim, &elem, point)};
  _fe->reinit(&elem, &master_points);

  // Need to reinit first so that we can get a size for _phi
  prepare(elem);

  const auto & phi = _fe->get_phi();
  mooseAssert(_current_accumulation.size() == phi.size(), "Not sized properly");
  for (const auto i : index_range(phi))
    _current_accumulation[i] += phi[i][0] * value;
}

void
NonZeroedAuxAccumulator::finalize()
{
  AccumulatorBase::finalize();
  _aux.solution().close();
  // The update was added here becuase without it the value was not properly set so that it was
  // accessible to vectorpostprocessors and postprocessors
  _aux.system().update();
}

void
NonZeroedAuxAccumulator::addCachedValues()
{
  std::vector<libMesh::dof_id_type> di;
  _aux.dofMap().dof_indices(&currentElem(), di, _var.number());

  mooseAssert(_current_accumulation.size() == di.size(), "Inconsistent size");
  for (const auto i : index_range(di))
    _aux.solution().add(di[i], _current_accumulation[i]);

  std::fill(_current_accumulation.begin(), _current_accumulation.end(), 0);
}

void
NonZeroedAuxAccumulator::initCachedValues()
{
  mooseAssert(std::find_if(_current_accumulation.begin(),
                           _current_accumulation.end(),
                           [](const auto & val)
                           { return val != 0; }) == _current_accumulation.end(),
              "Values not zeroed");

  mooseAssert(_fe->get_phi().size(), "Test function not sized");
  _current_accumulation.resize(_fe->get_phi().size(), 0);
}
}
