/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "SpatioTemporalPath.h"

InputParameters
SpatioTemporalPath::validParams()
{
  auto params = UserObject::validParams();
  params.addParam<Real>("FD_abs_eps",
                        1e-6,
                        "The absolute epsilon to use when using finite-differencing to "
                        "approximate the path's velocity and direction.");
  params.addParam<Real>("FD_rel_eps",
                        1e-6,
                        "The relative epsilon to use when using finite-differencing to "
                        "approximate the path's velocity and direction.");
  params.addParam<bool>(
      "verbose", false, "Set to true to print additional information about the path.");
  params.addParam<Real>(
      "update_interval",
      0,
      "Time interval betweem path front updates. This parameter controls how often the path front "
      "is updated. The default value (0) is equivalent to live update, i.e., at every time step "
      "the path position, velocity, and direction are recomputed.");

  params.addParam<bool>(
      "smoothing",
      false,
      "If the path is not smooth, the velocity and direction may experience abrupt change at "
      "vertices. Set this option to true to enable smoothing by averaging the direction and "
      "velocity over a time window specified FD_smoothing_time_window.");
  params.addParam<Real>("smoothing_time_window",
                        0,
                        "The window size used to smooth direction and velocity. See FD_smoothing "
                        "for explanation about smoothing.");
  params.addParam<unsigned int>(
      "smoothing_points", 20, "The number of sampling points used in smoothing.");

  // Only execute at INITIAL and TIMESTEP_BEGIN -- that's when the current time changes
  ExecFlagEnum execute_options = MooseUtils::getDefaultExecFlagEnum();
  execute_options = {EXEC_INITIAL, EXEC_TIMESTEP_BEGIN};
  params.set<ExecFlagEnum>("execute_on") = execute_options;
  params.suppressParameter<ExecFlagEnum>("execute_on");

  return params;
}

SpatioTemporalPath::SpatioTemporalPath(const InputParameters & params)
  : UserObject(params),
    TransientInterface(this),
    _verbose(getParam<bool>("verbose")),
    _abs_epsilon(getParam<Real>("FD_abs_eps")),
    _rel_epsilon(getParam<Real>("FD_rel_eps")),
    _interval(getParam<Real>("update_interval")),
    _last_updated(declareRestartableData<Real>("last_updated")),
    _smooth(getParam<bool>("smoothing")),
    _smooth_window(getParam<Real>("smoothing_time_window")),
    _smooth_points(getParam<unsigned int>("smoothing_points")),
    _current_position(std::numeric_limits<Real>::quiet_NaN(),
                      std::numeric_limits<Real>::quiet_NaN(),
                      std::numeric_limits<Real>::quiet_NaN()),
    _current_velocity(std::numeric_limits<Real>::quiet_NaN(),
                      std::numeric_limits<Real>::quiet_NaN(),
                      std::numeric_limits<Real>::quiet_NaN()),
    _current_direction(std::numeric_limits<Real>::quiet_NaN(),
                       std::numeric_limits<Real>::quiet_NaN(),
                       std::numeric_limits<Real>::quiet_NaN()),
    _previous_position(declareRestartableData<Point>("previous_position")),
    _previous_velocity(declareRestartableData<RealVectorValue>("previous_velocity")),
    _previous_direction(declareRestartableData<RealVectorValue>("previous_direction"))
{
}

void
SpatioTemporalPath::initialSetup()
{
  update();
}

void
SpatioTemporalPath::timestepSetup()
{
  if (_t > _last_updated + _interval)
    update();
}

void
SpatioTemporalPath::meshChanged()
{
  timestepSetup();
}

void
SpatioTemporalPath::update()
{
  _previous_position = _current_position;
  _previous_velocity = _current_velocity;
  _previous_direction = _current_direction;

  _current_position = position(_t);
  _current_velocity = _smooth ? smoothVelocity(_t) : velocity(_t);
  _current_direction = _smooth ? smoothDirection(_t) : direction(_t);

  _last_updated = _t;

  if (_verbose)
  {
    _console << "Spatio-temporal path: " << name() << std::endl;
    _console << "      position: " << position() << std::endl;
    _console << "      velocity: " << velocity() << std::endl;
    _console << "     direction: " << direction() << std::endl;
  }
}

RealVectorValue
SpatioTemporalPath::velocity(Real t) const
{
  auto dt = _rel_epsilon * std::abs(t);
  if (dt < _abs_epsilon)
    dt = _abs_epsilon;

  return (position(t + dt) - position(t)) / dt;
}

RealVectorValue
SpatioTemporalPath::smoothVelocity(Real t) const
{
  auto dt = _smooth_window / (_smooth_points - 1);
  RealVectorValue vel;
  for (auto i : make_range(_smooth_points))
    vel += velocity(t - _smooth_window / 2 + i * dt);
  return vel / _smooth_points;
}

RealVectorValue
SpatioTemporalPath::direction(Real t) const
{
  auto dt = _rel_epsilon * std::abs(t);
  if (dt < _abs_epsilon)
    dt = _abs_epsilon;

  auto dist = position(t + dt) - position(t);

  return dist.norm() == 0.0 ? RealVectorValue(0, 0, 0) : dist.unit();
}

RealVectorValue
SpatioTemporalPath::smoothDirection(Real t) const
{
  auto dt = _smooth_window / (_smooth_points - 1);

  // Averaging directions is equivalent to finding the dominant singular vector
  DenseMatrix<Real> dirs;
  dirs.resize(_smooth_points, 3);
  dirs.zero();

  // Fill out all directions (to be averaged)
  for (auto i : make_range(_smooth_points))
  {
    auto dir = direction(t - _smooth_window / 2 + i * dt);
    dirs(i, 0) = dir(0);
    dirs(i, 1) = dir(1);
    dirs(i, 2) = dir(2);
  };

  // SVD
  DenseVector<Real> sigma;
  DenseMatrix<Real> U, VT;
  dirs.svd(sigma, U, VT);

  return RealVectorValue(VT(0, 0), VT(0, 1), VT(0, 2));
}

Real
SpatioTemporalPath::tangentialDistance(Real t, const Point & p) const
{
  auto p0 = position(t);
  auto d = _smooth ? smoothDirection(t) : direction(t);
  return (p - p0) * d;
}

Real
SpatioTemporalPath::tangentialDistance(const Point & p) const
{
  auto p0 = position();
  auto d = direction();
  return (p - p0) * d;
}

Real
SpatioTemporalPath::normalDistance(Real t, const Point & p) const
{
  auto p0 = position(t);
  auto d = _smooth ? smoothDirection(t) : direction(t);
  auto dp = p - p0;
  auto dpt = (p - p0) * d;
  auto dpn = dp - dpt * d;
  return dpn.norm();
}

Real
SpatioTemporalPath::normalDistance(const Point & p) const
{
  auto p0 = position();
  auto d = direction();
  auto dp = p - p0;
  auto dpt = (p - p0) * d;
  auto dpn = dp - dpt * d;
  return dpn.norm();
}
