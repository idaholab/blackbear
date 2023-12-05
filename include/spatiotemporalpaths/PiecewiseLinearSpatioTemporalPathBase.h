/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "SpatioTemporalPath.h"

/**
 * @brief A general piecewise linear spatiotemporal description of path.
 *
 * The path consists of a set of line segments. The line segments must be continuous and are stored
 * as a series of times and vertex coordinates. The coordinate (front) and direction of the path can
 * be queried using `operator()` with the current time.
 */
class PiecewiseLinearSpatioTemporalPathBase : public SpatioTemporalPath
{
public:
  static InputParameters validParams();

  PiecewiseLinearSpatioTemporalPathBase(const InputParameters & params);

  virtual Point position(Real t) const override;

  /// Get the times associated with all vertices
  const std::vector<Real> & times() const { return _times; }
  /// Get the times associated with all vertices
  const std::vector<Real> & times() { return _times; }

  /// Get the coordinates for all vertices
  const std::vector<Point> & coords() const { return _coords; }

  /// Get the coordinates for all vertices
  const std::vector<Point> & coords() { return _coords; }

protected:
  /// Set coordinates from components
  virtual void
  setCoords(const std::vector<Real> & x, const std::vector<Real> & y, const std::vector<Real> & z);

  /// Check the validity of the path
  virtual void validate() const;

  /// Get the bounding indices for time \p t
  std::pair<unsigned int, unsigned int> getIntervalIndices(Real t) const;

  /// Times associated with all vertices, in ascending order
  std::vector<Real> _times;

  /// Coordinates for all vertices
  std::vector<Point> _coords;

  /// Tolerance of query time
  const Real _t_tol;

  /// Extrapolation method
  const MooseEnum _outside;
};
