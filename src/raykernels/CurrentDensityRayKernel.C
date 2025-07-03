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

#include "CurrentDensityRayKernel.h"

registerMooseObject("SalamanderApp", ADCurrentDensityRayKernel);

template <bool is_ad>
InputParameters
CurrentDensityRayKernelTempl<is_ad>::validParams()
{
  InputParameters params = GenericRayKernel<is_ad>::validParams();

  params.addClassDescription(
      "RayKernel for calculating the current source from particles while they are being traced.");
  params.addRangeCheckedParam<unsigned int>(
      "component", "component < 3", "The component of the current you are contributing to");
  return params;
}

template <bool is_ad>
CurrentDensityRayKernelTempl<is_ad>::CurrentDensityRayKernelTempl(const InputParameters & params)
  : GenericRayKernel<is_ad>(params),
    _component(this->template getParam<unsigned int>("component")),
    _charge_index(this->_study.getRayDataIndex("charge")),
    _weight_index(this->_study.getRayDataIndex("weight"))
{
  _velocity_index = _component == 0   ? this->_study.getRayDataIndex("v_x")
                    : _component == 1 ? this->_study.getRayDataIndex("v_y")
                                      : this->_study.getRayDataIndex("v_z");
}

template <bool is_ad>
GenericReal<is_ad>
CurrentDensityRayKernelTempl<is_ad>::computeQpResidual()
{
  const auto & ray = currentRay();
  return -ray->data(_charge_index) * ray->data(_weight_index) * ray->direction()(_component) *
         _test[_i][_qp] / this->_dt;
}

template class CurrentDensityRayKernelTempl<true>;
