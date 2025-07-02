//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ReflectParticleBC.h"
// Local includes
#include "RayTracingStudy.h"
#include <filesystem>

registerMooseObject("RayTracingApp", ReflectParticleBC);

InputParameters
ReflectParticleBC::validParams()
{
  auto params = ReflectRayBC::validParams();
  return params;
}

ReflectParticleBC::ReflectParticleBC(const InputParameters & params)
  : ReflectRayBC(params),
    _mesh_dimension(_fe_problem.mesh().dimension()),
    _velocity_indicies({_study.getRayDataIndex("v_x"),
                        _study.getRayDataIndex("v_y"),
                        _study.getRayDataIndex("v_z")})
{
}

void
ReflectParticleBC::onBoundary(const unsigned int num_applying)
{
  // reflect the ray normally and then we will also update the data
  ReflectRayBC::onBoundary(num_applying);
  for (const auto i : make_range(_mesh_dimension))
  {
    _temporary_velocity(i) = currentRay()->data(_velocity_indicies[i]);
  }
  _temporary_velocity = _temporary_velocity.norm() * currentRay()->direction();

  for (const auto i : make_range(_mesh_dimension))
  {
    currentRay()->data(_velocity_indicies[i]) = _temporary_velocity(i);
  }
}
