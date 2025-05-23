//* This file is part of SALAMANDER: Software for Advanced Large-scale Analysis of MAgnetic confinement for Numerical Design, Engineering & Research,
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

#include "Moose.h"
#include "MooseUtils.h"
#include "PeriodicPICStudy.h"
#include "ParticleStepperBase.h"
#include "ClaimRays.h"
#include <algorithm>
#include <cmath>
#include <limits>

registerMooseObject("SalamanderApp", PeriodicPICStudy);

InputParameters
PeriodicPICStudy::validParams()
{
  auto params = TestInitializedPICStudy::validParams();
  return params;
}

PeriodicPICStudy::PeriodicPICStudy(const InputParameters & parameters)
  : TestInitializedPICStudy(parameters),
  _x_min(std::numeric_limits<float>::max()),
  _x_max(std::numeric_limits<float>::lowest()),
  _domain_length(0)
{ 
  for (const auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    _domain_length += elem->volume();
    for (const auto & node : elem->node_ref_range())
    {
      if (node(0) < _x_min)
        _x_min = node(0);

      if (node(0) > _x_max)
        _x_max = node(0);
    }
  }
  
  _local_x_max = _x_max;
  _local_x_min = _x_min;
  comm().sum(_domain_length);
  comm().min(_x_min);
  comm().max(_x_max);
}
void
PeriodicPICStudy::reinitializeParticles()
{
  // Reset each ray
  for (auto & ray : _banked_particles)
  {
    // Store off the ray's info before we reset it
    const auto elem = ray->currentElem();
    const auto point = ray->currentPoint();
    const auto distance = ray->distance();

    getVelocity(*ray, _temporary_velocity);
    // Reset it (this is required to reuse a ray)
    ray->resetCounters();
    ray->clearStartingInfo();

    // And set the new starting information
    ray->setStart(point, elem);
    _stepper.setupStep(
        *ray, _temporary_velocity, ray->data()[_charge_index] / ray->data()[_mass_index], distance);

    setVelocity(*ray, _temporary_velocity);
  }

  if (_periodic_particles.empty())
    return;

  for (const auto i : make_range(_periodic_particles.size()))
  {
    auto & ray = _banked_particles.emplace_back(acquireRay());
    setInitialParticleData(ray, _periodic_particles[i]);
    getVelocity(*ray, _temporary_velocity);
    _stepper.setupStep(
        *ray, _temporary_velocity, ray->data(_charge_index) / ray->data(_mass_index));
    setVelocity(*ray, _temporary_velocity);
  }
}

void
PeriodicPICStudy::postExecuteStudy() {  

  _periodic_particles.clear(); 
  _banked_particles = rayBank(); 

  // we have to store all this data in seperate vectors since 
  // TIMPI does not support allgather with structs
  std::vector<Real> x_pos, mass, charge, weight, vx, vy, vz; 
  std::vector<int> species; 

  _banked_particles.erase(
    std::remove_if(
      _banked_particles.begin(),
      _banked_particles.end(),
      [&](const std::shared_ptr<Ray> & ray)
      {
        // check if any of the particles stopped early in tracing 
        // the only reason they should have stopped early is becuase they hit a
        // boundary of the domain and as a result need to be restarted within a different element
        if (MooseUtils::absoluteFuzzyEqual(ray->maxDistance(), ray->distance())||  ray->stationary())
          return false;

        mass.push_back(ray->data(_mass_index)); 
        charge.push_back(ray->data(_charge_index)); 
        weight.push_back(ray->data(_weight_index)); 
        species.push_back(ray->data(_species_index)); 
        vx.push_back(ray->data(_v_x_index)); 
        vy.push_back(ray->data(_v_y_index)); 
        vz.push_back(ray->data(_v_z_index)); 

        if (std::signbit(ray->direction()(0))) 
        {
          x_pos.push_back(_x_max - std::abs(std::fmod(ray->maxDistance() - ray->distance(), _domain_length)));
        }
        else 
        {
          x_pos.push_back(_x_min + std::abs(std::fmod(ray->maxDistance() - ray->distance(), _domain_length)));
        }

        return true; 
      }),
   _banked_particles.end());

  // gather all of the data from all the ranks 
  // we need to do this since a periodic particle could be anywhere in the domain 
  comm().allgather(x_pos); 
  comm().allgather(mass); 
  comm().allgather(charge); 
  comm().allgather(species); 
  comm().allgather(weight); 
  comm().allgather(vx); 
  comm().allgather(vy); 
  comm().allgather(vz); 

  // we'll go ahead and remove all the particles that should not be on this processor
  unsigned int index = 0;

  auto particle_filter = [&index, x_pos, this](auto)
  {
    bool result = (x_pos[index] < _local_x_min || x_pos[index] > _local_x_max);
    index++; 
    return result;
  };

  mass.erase(std::remove_if(mass.begin(),mass.end(),particle_filter), mass.end());
  index = 0;
  charge.erase(std::remove_if(charge.begin(),charge.end(),particle_filter), charge.end());
  index = 0;
  species.erase(std::remove_if(species.begin(),species.end(),particle_filter), species.end());
  index = 0;
  weight.erase(std::remove_if(weight.begin(),weight.end(),particle_filter), weight.end());
  index = 0;
  vx.erase(std::remove_if(vx.begin(),vx.end(),particle_filter), vx.end());
  index = 0;
  vy.erase(std::remove_if(vy.begin(),vy.end(),particle_filter), vy.end());
  index = 0;
  vz.erase(std::remove_if(vz.begin(),vz.end(),particle_filter), vz.end());
  index = 0;
  x_pos.erase(std::remove_if(x_pos.begin(),x_pos.end(),
      [&](const Real x)
      {
        return x < _local_x_min || x > _local_x_max;
      }), x_pos.end());

  // putting all this data back into the ParticleData struct 
  // this is so that we can accumulate properties later when needed
  _periodic_particles.resize(x_pos.size());
  for (const auto i : make_range(x_pos.size()))
  { 
    _periodic_particles[i].elem = nullptr; 
    _periodic_particles[i].position(0) = x_pos[i]; 
    _periodic_particles[i].mass = mass[i]; 
    _periodic_particles[i].charge = charge[i]; 
    _periodic_particles[i].species = species[i]; 
    _periodic_particles[i].weight = weight[i]; 
    _periodic_particles[i].velocity(0) = vx[i]; 
    _periodic_particles[i].velocity(1) = vy[i]; 
    _periodic_particles[i].velocity(2) = vz[i]; 
  }

  for (auto & data : _periodic_particles)
  {
    for (auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
    {
      if (elem->contains_point(data.position))
      {
        data.elem = elem;
      }
    }
  }
}
