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

#include "CollisionalPICStudy.h"
#include "MooseRandom.h"
#include "RankTwoTensor.h"

registerMooseObject("SalamanderApp", CollisionalPICStudy);

InputParameters
CollisionalPICStudy::validParams()
{
  auto params = PICStudyBase::validParams();
  params.addRequiredParam<Real>("cross_section", "The constant cross section for collisions");
  params.addParam<unsigned int>("seed", 0, "The seed value for the random number generator");
  return params;
}

CollisionalPICStudy::CollisionalPICStudy(const InputParameters & parameters)
  : PICStudyBase(parameters), _cross_section(getParam<Real>("cross_section"))
{
  _generator.seed(getParam<unsigned int>("seed"));
  const auto & elem_range = *_fe_problem.mesh().getActiveLocalElementRange();
  const auto num_local_elements = std::distance(elem_range.begin(), elem_range.end());
  _particle_indicies.resize(num_local_elements);
}

void
CollisionalPICStudy::postExecuteStudy()
{
  // clears all of rays that are dead
  PICStudyBase::postExecuteStudy();

  unsigned int elem_idx;
  std::vector<unsigned int> elem_ids;
  for (const auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
    elem_ids.push_back(elem->id());
  // collect all of the particles in each element
  for (const auto i : make_range(_banked_particles.size()))
  {
    elem_idx = 0;
    for (const auto id : elem_ids)
    {
      if (id == _banked_particles[i]->currentElem()->id())
      {
        _particle_indicies[elem_idx].push_back(i);
        break;
      }
      elem_idx++;
    }
  }

  // this assumes maxwell molecules with
  // a cross section of 1
  Real sigma_cr_max = _cross_section;
  Real sigma_cr_temp = _cross_section;
  unsigned int index_1, index_2;
  Point v1, v2;
  for (auto & indicies : _particle_indicies)
  {
    if (indicies.size() < 2)
      continue;

    auto volume = _banked_particles[indicies.front()]->currentElem()->volume();
    //  for now we are assuming a constant particle weight
    //  Fn is birds notation
    auto Fn = _banked_particles[indicies.front()]->data(_weight_index);
    //    Real temp_pairs = 0.5 * indicies.size() * indicies.size() * sigma_cr_max * Fn * _dt /
    //    volume +
    //                      _generator.rand();
    Real temp_pairs =
        0.5 * indicies.size() * indicies.size() * sigma_cr_max * Fn * _dt / volume + 0.5;

    unsigned int pairs = temp_pairs;
    for (const auto i [[maybe_unused]] : make_range(pairs))
    {
      index_1 = indicies[(unsigned int)(indicies.size() * _generator.rand())];
      do
      {
        index_2 = indicies[(unsigned int)(indicies.size() * _generator.rand())];
      } while (index_1 == index_2);
      // convert from indicies index to _banked_particles index

      Point v1 = getVelocity(_banked_particles[index_1]);
      Real m1 = _banked_particles[index_1]->data(_mass_index);

      Point v2 = getVelocity(_banked_particles[index_2]);
      Real m2 = _banked_particles[index_2]->data(_mass_index);

      Point cr = v1 - v2;
      Real cr_mag = cr.norm();
      Point cm = (m1 * v1 + m2 * v2) / (m1 + m2);
      // we are getting setting this to the same as the cross section
      // this will assume maxwell molecules
      // sigma_cr_temp = cr.norm() * _cross_section;

      if (sigma_cr_temp / sigma_cr_max < _generator.rand())
        continue;

      Real cos_chi = 2 * _generator.rand() - 1;
      Real sin_chi = std::sqrt(1 - cos_chi * cos_chi);
      Real eps = 2 * M_PI * _generator.rand();

      cr(0) = cr_mag * cos_chi;
      cr(1) = cr_mag * sin_chi * std::cos(eps);
      cr(2) = cr_mag * sin_chi * std::sin(eps);

      v1 = cm + m2 / (m1 + m2) * cr;
      v2 = cm - m1 / (m1 + m2) * cr;

      setVelocity(*_banked_particles[index_1], v1);
      setVelocity(*_banked_particles[index_2], v2);
    }

    // clear all of the particle indicies ones we
    // have collided all of the particles in the element
    indicies.clear();
  }
}
