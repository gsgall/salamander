//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "CustomHistogramVectorPostprocessor.h"

#include <algorithm>

registerMooseObject("MooseApp", CustomHistogramVectorPostprocessor);

InputParameters
CustomHistogramVectorPostprocessor::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();
  params.addClassDescription("Compute a histogram for each column of a VectorPostprocessor");

  params.addRequiredParam<VectorPostprocessorName>(
      "vpp", "The VectorPostprocessor to compute histogram of");

  params.addRequiredParam<unsigned int>("num_bins", "The number of bins for the histograms");

  params.addParam<std::vector<std::string>>("column_names",
                                            "the name of the columns in the vector postprocessor");
  params.addRequiredParam<Real>("lower_bound", "The lower bound of the histogram");
  params.addRequiredParam<Real>("upper_bound", "The upper bound of the histogram");
  return params;
}

CustomHistogramVectorPostprocessor::CustomHistogramVectorPostprocessor(
    const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _vpp_name(getParam<VectorPostprocessorName>("vpp")),
    _num_bins(getParam<unsigned int>("num_bins")),
    _lower_bound(getParam<Real>("lower_bound")),
    _upper_bound(getParam<Real>("upper_bound")),
    _column_names(isParamSetByUser("column_names")
                      ? getParam<std::vector<std::string>>("column_names")
                      : [&]() {
  const VectorPostprocessor & vpp = getUserObjectByName<VectorPostprocessor>(_vpp_name);
  std::vector<std::string> names;
  for (const auto & name : vpp.getVectorNames())
    names.push_back(name);
                      return names; }())
{
}

void
CustomHistogramVectorPostprocessor::initialSetup()
{
  for (const auto & vec_name : _column_names)
    _histogram_data[vec_name] = {&declareVector(vec_name + "_lower"),
                                 &declareVector(vec_name + "_upper"),
                                 &declareVector(vec_name)};
  if (_histogram_data.empty())
    paramError("vpp", "The specified VectorPostprocessor does not have any declared vectors");
}

void
CustomHistogramVectorPostprocessor::initialize()
{
  // no need to reset, execute() writes in place
}

void
CustomHistogramVectorPostprocessor::execute()
{
  if (processor_id() == 0) // Only compute on processor 0
  {
    for (const auto & vec_name : _column_names)
    {
      const auto & values = _fe_problem.getVectorPostprocessorValueByName(_vpp_name, vec_name);

      mooseAssert(_histogram_data.count(vec_name), "Error retrieving VPP vector");
      auto & histo_data = _histogram_data.at(vec_name);
      computeHistogram(values, histo_data);
    }
  }
}

void
CustomHistogramVectorPostprocessor::finalize()
{
}

void
CustomHistogramVectorPostprocessor::computeHistogram(const std::vector<Real> & values,
                                                     HistoData & histo_data)
{
  if (values.empty())
    mooseError("Cannot compute histogram without data!");

  // Grab the vectors to fill
  auto & lower_vector = *histo_data._lower;
  auto & upper_vector = *histo_data._upper;
  auto & histogram = *histo_data._histogram;

  // Resize everything
  // Note: no need to zero anything out
  // that will automatically be done if the bin should be zero by the algorithm below
  lower_vector.resize(_num_bins);
  upper_vector.resize(_num_bins);
  histogram.resize(_num_bins);

  // Create a sorted copy of the values
  std::vector<Real> sorted_values(values.size());
  std::partial_sort_copy(values.begin(), values.end(), sorted_values.begin(), sorted_values.end());

  // Get the min and max values

  // The bin stride/length
  auto bin_stride = (_upper_bound - _lower_bound) / static_cast<Real>(_num_bins);

  auto current_value_iter = sorted_values.begin();
  auto sorted_values_end = sorted_values.end();

  // Fill the bins
  for (unsigned int bin = 0; bin < _num_bins; bin++)
  {
    // Compute bin edges
    // These are computed individually on purpose so that the exact same values will match the
    // previous and next bins
    auto lower = (bin * bin_stride) + _lower_bound;
    auto upper = ((bin + 1) * bin_stride) + _lower_bound;

    lower_vector[bin] = lower;
    upper_vector[bin] = upper;

    // Find the number of values that fall in this bin
    unsigned long int num_values = 0;
    while (current_value_iter != sorted_values_end && *current_value_iter <= upper)
    {
      num_values++;
      current_value_iter++;
    }

    histogram[bin] = static_cast<Real>(num_values);
  }
}
