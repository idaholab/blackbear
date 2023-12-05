/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "PiecewiseLinearSpatioTemporalPathBase.h"

InputParameters
PiecewiseLinearSpatioTemporalPathBase::validParams()
{
  auto params = SpatioTemporalPath::validParams();
  params.addParam<Real>("time_tolerance", 5e-12, "Tolerance of query time");
  MooseEnum outside_behavior("CONSTANT EXTRAPOLATION EXCEPTION", "CONSTANT");
  params.addParam<MooseEnum>(
      "outside",
      outside_behavior,
      "The method to use when extrapolating the path position outside its temporal support. "
      "CONSTANT: Return the closest path point; EXTRAPOLATION: Linear extrapolation; EXCEPTION: "
      "Raise an exception when trying to extrapolate.");
  return params;
}

PiecewiseLinearSpatioTemporalPathBase::PiecewiseLinearSpatioTemporalPathBase(
    const InputParameters & params)
  : SpatioTemporalPath(params),
    _t_tol(getParam<Real>("time_tolerance")),
    _outside(getParam<MooseEnum>("outside"))
{
}

Point
PiecewiseLinearSpatioTemporalPathBase::position(Real t) const
{
  if (t < (_times[0] - _t_tol) || t > (_times.back() + _t_tol))
  {
    if (_outside == "EXCEPTION")
      mooseException("A spatiotemporal path is queried outside its temporal support.");
    else if (_outside == "CONSTANT")
    {
      if (t < _times[0])
        return _coords[0];
      if (t > _times.back())
        return _coords.back();
    }
    else if (_outside == "EXTRAPOLATION")
    {
      // no-op
    }
    else
      paramError("extrapolation_method", "Unsupported extrapolation method.");
  }

  auto [i1, i2] = getIntervalIndices(t);

  // Linearly interpolate the coordinates
  auto p1 = _coords[i1];
  auto p2 = _coords[i2];
  auto fraction = (t - _times[i1]) / (_times[i2] - _times[i1]);
  auto p = p1 + fraction * (p2 - p1);

  if (std::isnan(p(0)) || std::isnan(p(1)) || std::isnan(p(2)))
    mooseException("Encountered NaN when querying a spatiotemporal path.");

  return p;
}

void
PiecewiseLinearSpatioTemporalPathBase::setCoords(const std::vector<Real> & x,
                                                 const std::vector<Real> & y,
                                                 const std::vector<Real> & z)
{
  // Size check
  auto sz = std::max({x.size(), y.size(), z.size()});
  if (sz == 0 || (!x.empty() && x.size() != sz) || (!x.empty() && x.size() != sz) ||
      (!x.empty() && x.size() != sz))
    mooseError(
        "Error while constructing a spatio-temporal path: At lease one of the x-, y-, and z- "
        "coordinates must be non-empty, and the non-zero sizes must match. x-coordinates have "
        "size ",
        x.size(),
        ", y-coordinates have size ",
        y.size(),
        ", z-coordinates have size ",
        z.size(),
        ".");

  // Assigne coordinates
  _coords.resize(sz);
  for (auto i : make_range(sz))
  {
    _coords[i](0) = x.empty() ? 0.0 : x[i];
    _coords[i](1) = y.empty() ? 0.0 : y[i];
    _coords[i](2) = z.empty() ? 0.0 : z[i];
  }
}

void
PiecewiseLinearSpatioTemporalPathBase::validate() const
{
  // Time and coords must have same size
  if (_times.size() != _coords.size())
    mooseError("Error while constructing a spatio-temporal path: The time series and the "
               "coordinates must have the same size. The time series have size ",
               _times.size(),
               ", and the coordinates have size ",
               _coords.size(),
               ".");

  // There must be at least one line segment
  if (_times.size() < 2)
    mooseError("Error while constructing a spatio-temporal path: There must be at least two "
               "vertices on the path, but only ",
               _times.size(),
               " is provided.");

  // Time must be non-decreasing
  for (auto i : index_range(_times))
    if (i > 0 && _times[i] < _times[i - 1])
      mooseError("Error while constructing a spatio-temporal path: The time series must be "
                 "non-decreasing. The time at index ",
                 i - 1,
                 " is ",
                 _times[i - 1],
                 ", and the time at index ",
                 i,
                 " is ",
                 _times[i],
                 ".");
}

std::pair<unsigned int, unsigned int>
PiecewiseLinearSpatioTemporalPathBase::getIntervalIndices(Real t) const
{
  if (t < _times[0])
    return {0, 1};

  for (auto i : index_range(_times))
    if (t >= _times[i])
    {
      if (i == _times.size() - 1)
        return {_times.size() - 2, _times.size() - 1};
      else
        return {i, i + 1};
    }

  mooseError("Failed to query a spatiotemporal path with time ",
             t,
             ". If you believe the query is valid, try increasing the time_tolerance as query may "
             "fail due to machine precision.");
  return {0, 0};
}
