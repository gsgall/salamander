//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Ray.h"
#include "ReflectRayBC.h"

class ReflectParticleBC : public ReflectRayBC
{
public:
  ReflectParticleBC(const InputParameters & params);

  static InputParameters validParams();

  virtual void onBoundary(const unsigned int num_applying) override;

protected:
  const unsigned int _mesh_dimension;
  const std::vector<RayDataIndex> _velocity_indicies;
  Point _temporary_velocity;
  Point _temporary_direction;
};
