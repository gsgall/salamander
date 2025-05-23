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

#include "MooseError.h"
#include "PICStudyBase.h"
#include "ParticleStepperBase.h"
#include <algorithm>

InputParameters
PICStudyBase::validParams()
{
  auto params = RayTracingStudy::validParams();
  params.addClassDescription("Base class for PIC studies. Provides some of the basic ray data "
                             "needed for particle tracking."
                             "Basic logic for resetting rays to be used as particles after the "
                             "original generation is complete is also provided");
  params.addRequiredParam<UserObjectName>(
      "stepper",
      "The ParticleStepper UserObject that has the rules for how particle"
      "velocities should be updated");
  params.addRequiredParam<std::vector<UserObjectName>>("initializers",
                                          "The initializer that will place particles");
  // We're not going to use registration because we don't care to name our rays because
  // we will have a lot of them
  params.set<bool>("_use_ray_registration") = false;

  return params;
}

PICStudyBase::PICStudyBase(const InputParameters & parameters)
  : RayTracingStudy(parameters),
    _banked_particles(
        declareRestartableDataWithContext<std::vector<std::shared_ptr<Ray>>>("_banked_particles", this)),
    _v_x_index(registerRayData("v_x")),
    _v_y_index(registerRayData("v_y")),
    _v_z_index(registerRayData("v_z")),
    _weight_index(registerRayData("weight")),
    _charge_index(registerRayData("charge")),
    _mass_index(registerRayData("mass")),
    _species_index(registerRayData("species")),
    _stepper(getUserObject<ParticleStepperBase>("stepper")),
    _has_generated(declareRestartableData<bool>("has_generated", false)),
    _initializer_names(getParam<std::vector<UserObjectName>>("initializers"))

{
  if (_initializer_names.empty())
    paramError("intializers", "At least one initializer must be provided"); 
  for (const auto name : _initializer_names) 
    _initializers.push_back(&getUserObjectByName<ParticleInitializerBase>(name));
}

void
PICStudyBase::generateRays()
{
  // We generate rays the first time only, after that we will
  // pull from the bank and update velocities/max distances
  if (!_has_generated)
  {
    this->initializeParticles();
    _has_generated = true;
  }
  else
  {
    reinitializeParticles();
    // Add the rays to be traced
    moveRaysToBuffer(_banked_particles);
    _banked_particles.clear();
  }
}


std::shared_ptr<Ray>
PICStudyBase::createParticle(const InitialParticleData & data)
{
  auto ray = acquireRay();
  setInitialParticleData(ray, data);
  getVelocity(*ray, _temporary_velocity);
  _stepper.setupStep(
      *ray, _temporary_velocity, ray->data(_charge_index) / ray->data(_mass_index));
  setVelocity(*ray, _temporary_velocity);
  return ray;
}

void 
PICStudyBase::initializeParticles()
{
  std::vector<InitialParticleData> initial_data; 
  // collect all of the data for all the various types of particles that will exist
  for (const auto initializer : _initializers)
  {
    const auto temporary_data = initializer->getParticleData();
    initial_data.insert(initial_data.end(), temporary_data.begin(), temporary_data.end()); 
  }
  // if this processor doesn't have any paricles we don't need to do anything else
  if (initial_data.size() == 0)
    return;
  _banked_particles.resize(initial_data.size());

  for (unsigned int i = 0; i < initial_data.size(); ++i)
  {
    _banked_particles[i] = createParticle(initial_data[i]);
  }
  moveRaysToBuffer(_banked_particles);
}

void
PICStudyBase::reinitializeParticles()
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
}

void
PICStudyBase::postExecuteStudy()
{
  // we are going to be re using the same rays which just took a step so
  // we store them here to reset them in the generateRays method
  _banked_particles = rayBank();
  // removing all of the rays which were killed during their tracing
  _banked_particles.erase(std::remove_if(_banked_particles.begin(),
                                    _banked_particles.end(),
                                    [](const std::shared_ptr<Ray> & ray)
                                    {
                                      if (ray->stationary())
                                        return false;

                                      return std::abs(ray->distance() - ray->maxDistance()) /
                                                 ray->maxDistance() >
                                             1e-6;
                                    }),
                     _banked_particles.end());
}

void
PICStudyBase::getVelocity(const Ray & ray, Point & v) const
{
  v(0) = ray.data(_v_x_index);
  v(1) = ray.data(_v_y_index);
  v(2) = ray.data(_v_z_index);
}

void
PICStudyBase::setVelocity(Ray & ray, const Point & v) const
{
  ray.data(_v_x_index) = v(0);
  ray.data(_v_y_index) = v(1);
  ray.data(_v_z_index) = v(2);
}

void
PICStudyBase::setInitialParticleData(std::shared_ptr<Ray> & ray, const InitialParticleData & data)
{
  mooseAssert(data.elem != nullptr, "Cannot create particle since the provided starting element is null");
  ray->setStart(data.position, data.elem);
  ray->data(_v_x_index) = data.velocity(0);
  ray->data(_v_y_index) = data.velocity(1);
  ray->data(_v_z_index) = data.velocity(2);
  ray->data(_mass_index) = data.mass;
  ray->data(_weight_index) = data.weight;
  ray->data(_charge_index) = data.charge;
}
