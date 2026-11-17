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

#include "OneDPerturbedDensityParticleInitializer.h"
#include "MooseRandom.h"
#include "ElementSampler.h"
#include <algorithm>
#include <limits>
registerMooseObject("SalamanderApp", OneDPerturbedDensityParticleInitializer);

InputParameters
OneDPerturbedDensityParticleInitializer::validParams()
{
  auto params = ParticleInitializerBase::validParams();
  params.addClassDescription(
      "Particle initializer that uniformly distributes a specified number of particles per "
      "element and calculates the corresponding particle weight based on the requested number "
      "density, particles per element, and the elements \"volume\".");
  params.addRangeCheckedParam<unsigned int>(
      "particles_per_element",
      "particles_per_element != 0",
      "The number of computational particles that should be placed in each element");

  params.addRangeCheckedParam<Real>("number_density",
                                    "number_density > 0.0",
                                    "The number density of particles you want to represent");
  params.addRangeCheckedParam<Real>("perturbation_prefactor",
                                    "perturbation_prefactor > 0.0",
                                    "The perturbation factor must be positive");
  params.addRangeCheckedParam<Real>("instability_mode",
                                            "instability_mode > 0",
                                            "The mode numer of the perturbation which will cause the instability");
  return params;
}

OneDPerturbedDensityParticleInitializer::OneDPerturbedDensityParticleInitializer(const InputParameters & parameters)
  : ParticleInitializerBase(parameters),
    _number_density(getParam<Real>("number_density")),
    _particles_per_element(getParam<unsigned int>("particles_per_element")),
    _perturbation_prefactor(getParam<Real>("perturbation_prefactor")),
    _instability_mode(getParam<Real>("instability_mode"))
{
  if (_mesh_dimension != 1)
    mooseError("This particle initializer is only designed to be used with a one dimensional mesh.");

  _num_local_elems = 0;
  _xmin = std::numeric_limits<float>::max();
  _xmax = std::numeric_limits<float>::min();
  _domain_length = 0;
  for (const auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    if (_xmin > (*elem->get_nodes()[0])(0))
      _xmin = (*elem->get_nodes()[0])(0);

    if (_xmax < (*elem->get_nodes()[1])(0))
      _xmax = (*elem->get_nodes()[1])(0);

    _domain_length += elem->volume();
    _num_local_elems++;
  }
  _num_total_elems = _num_local_elems;
  _local_xmin = _xmin;
  _local_xmax = _xmax;
  comm().min(_xmin);
  comm().max(_xmax);
  comm().sum(_num_total_elems);
  comm().sum(_domain_length);
}

std::vector<InitialParticleData>
OneDPerturbedDensityParticleInitializer::getParticleData() const
{
  // counting the number of elements this process is responsible for
  // this will allow us to allocated data structures of the appropriate length
  // if there are no elements for this processor: do nothing
  if (_num_local_elems == 0)
    return {};

  Real weight = _number_density * _domain_length / (_particles_per_element * _num_total_elems);

  auto points = std::vector<Point>(_particles_per_element * _num_total_elems);
  Real dx = _domain_length / (_num_total_elems * _particles_per_element);
  for (const auto i : make_range(points.size()))
  {
    Real x = (i + 0.5) * dx;
    x += _perturbation_prefactor * std::cos(2 * M_PI * _instability_mode / _domain_length * x);
    if (x > _xmax) {
      x -= _domain_length;
    } else if (x < _xmin)
    {
      x += _domain_length;
    }
    points[i](0) = x;
  }

  points.erase(
    std::remove_if(
      points.begin(),
      points.end(),
      [this](const Point & p)
      {
        return p(0) < _local_xmin || p(0) > _local_xmax;
      }),
      points.end());

  std::sort(points.begin(), points.end());
  auto data = std::vector<InitialParticleData>(points.size());
  const auto & velocities = _velocity_initializer.getParticleVelocities(points.size());

  unsigned int particle_index = 0;
  MooseRandom generator;
  generator.seed(_seed);
  for (const auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    while(particle_index < points.size() && elem->contains_point(points[particle_index]))
    {
      data[particle_index].elem = elem;
      data[particle_index].weight = weight;
      data[particle_index].position = points[particle_index];
      data[particle_index].velocity = velocities[particle_index];
      particle_index++;
     }
  }
  return data;
}
