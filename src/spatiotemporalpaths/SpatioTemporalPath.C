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
SpatioTemporalPath::update()
{
  _previous_position = _current_position;
  _previous_velocity = _current_velocity;
  _previous_direction = _current_direction;

  _current_position = position(_t);
  _current_velocity = velocity(_t);
  _current_direction = direction(_t);

  _last_updated = _t;

  if (_verbose)
  {
    _console << "Spatio-temporal path: " << name() << std::endl;
    _console << "      position: " << position(_t) << std::endl;
    _console << "      velocity: " << velocity(_t) << std::endl;
    _console << "     direction: " << direction(_t) << std::endl;
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
SpatioTemporalPath::direction(Real t) const
{
  auto dt = _rel_epsilon * std::abs(t);
  if (dt < _abs_epsilon)
    dt = _abs_epsilon;

  auto dist = position(t + dt) - position(t);

  return dist.norm() == 0.0 ? RealVectorValue(0, 0, 0) : dist.unit();
}

Real
SpatioTemporalPath::tangentialDistance(Real t, const Point & p) const
{
  auto p0 = position(t);
  auto d = direction(t);
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
  auto d = direction(t);
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
