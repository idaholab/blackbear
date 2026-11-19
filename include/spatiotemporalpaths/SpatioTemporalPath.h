/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                       BlackBear                              */
/*                                                              */
/*           (c) 2017 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#pragma once

#include "GeneralUserObject.h"
#include "LinearInterpolation.h"

/**
 * A piecewise linear spatio-temporal path x(t). The path vertices are given either directly
 * (t, x, y, z) or from a CSV file. The path position is linearly interpolated in time, and the
 * direction is the unit vector of the current segment.
 */
class SpatioTemporalPath : public GeneralUserObject
{
public:
  static InputParameters validParams();

  SpatioTemporalPath(const InputParameters & params);

  virtual void initialize() override {}
  virtual void execute() override {}
  virtual void finalize() override {}

  /// Update the current position and direction. Called before any object executes, so every
  /// object sees the path at the current time.
  virtual void initialSetup() override { update(); }
  virtual void timestepSetup() override { update(); }

  /// Path position at time t
  Point position(Real t) const;
  /// Unit direction of the path segment active at time t (zero if the path is stationary)
  RealVectorValue direction(Real t) const;

  /// Position at the current time
  const Point & position() const { return _position; }
  /// Direction at the current time
  const RealVectorValue & direction() const { return _direction; }

  /// Distance from p to the current position, along the current direction
  Real tangentialDistance(const Point & p) const;
  /// Distance from p to the current position, perpendicular to the current direction
  Real normalDistance(const Point & p) const;

private:
  void update();

  /// One interpolator per coordinate, as functions of time
  std::array<LinearInterpolation, 3> _coord;

  Point _position;
  RealVectorValue _direction;
};
