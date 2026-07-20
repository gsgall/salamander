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

#include "AuxAccumulator.h"

#include "FEProblemBase.h"
#include "AuxiliarySystem.h"
#include "MooseVariableFE.h"
#include "MooseMesh.h"

namespace SALAMANDER
{
AuxAccumulator::AuxAccumulator(FEProblemBase & problem, const AuxVariableName & variable)
  : NonZeroedAuxAccumulator(problem, variable)
{
  _fe->request_phi();

  // Zero the solution before we accumulate
  std::vector<libMesh::dof_id_type> di;
  for (const auto & elem : *_problem.mesh().getActiveLocalElementRange())
  {
    _aux.dofMap().dof_indices(elem, di, _var.number());
    for (const auto i : di)
      _aux.solution().set(i, 0);
  }
  _aux.solution().close();
  // The update was added here becuase without it the value was not properly set so that it was
  // accessible to vectorpostprocessors and postprocessors
  _aux.system().update();
}
}
