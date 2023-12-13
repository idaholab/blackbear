/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#pragma once

#include "UserObject.h"
#include "libmesh/vector_value.h"
#include "TransientInterface.h"

/**
 * @brief General abstract description of a spatiotemporal path.
 *
 * Subclasses must define the query `operator()`. The coordinate (front) and direction of the path
 * can be queried using `operator()` with the current time.
 */
class SpatioTemporalPath : public UserObject, public TransientInterface
{
public:
  static InputParameters validParams();

  SpatioTemporalPath(const InputParameters & params);

  virtual void execute() override final {}
  virtual void initialize() override final {}
  virtual void finalize() override final {}
  virtual void threadJoin(const UserObject &) override final {}
  virtual void initialSetup() override;
  virtual void timestepSetup() override;
  virtual void meshChanged() override;

  /// Update the path's current position, velocity, direction, etc.
  virtual void update();

  /// Last time the path front was updated
  Real lastUpdated() const { return _last_updated; }

  /**
   * @brief Get the path coordinates at time \p t
   *
   * @param t The time to query
   * @return Point The path coordinates at time \p t
   */
  virtual Point position(Real t) const = 0;

  /// Get the current coordinates
  const Point & position() const { return _current_position; }

  /// Get the previous coordinates
  const Point & previousPosition() const { return _previous_position; }

  /**
   * @brief Get the path velocity at time \p t
   *
   * A default implementation is provided to use finite-differencing to compute the path velocity.
   * A subclass may choose to override this default implementation, and the input parameters
   * "path_FD_abs_eps" and "path_FD_rel_eps" should be suppressed.
   *
   * @param t The time to query
   * @return RealVectorValue The path velocity at time \p t
   */
  virtual RealVectorValue velocity(Real t) const;

  /// Get the current velocity
  const RealVectorValue & velocity() const { return _current_velocity; }

  /// Compute the average velocity
  RealVectorValue smoothVelocity(Real t) const;

  /// Get the previous velocity
  const RealVectorValue & previousVelocity() const { return _previous_velocity; }

  /**
   * @brief Get the path direction at time \p t
   *
   * A default implementation is provided to use finite-differencing to compute the path direction.
   * A subclass may choose to override this default implementation, and the input parameters
   * "path_FD_abs_eps" and "path_FD_rel_eps" should be suppressed.
   *
   * @param t The time to query
   * @return RealVectorValue The path direction at time \p t
   */
  virtual RealVectorValue direction(Real t) const;

  /// Get the current direction
  const RealVectorValue & direction() const { return _current_direction; }

  /// Compute the average direction
  RealVectorValue smoothDirection(Real t) const;

  /// Get the previous direction
  const RealVectorValue & previousDirection() const { return _previous_direction; }

  /// Get the tangential component of the distance between the given point \p p and the path's position at time \p t
  Real tangentialDistance(Real t, const Point & p) const;

  /// Get the tangential component of the distance between the given point \p p and the path's current position
  Real tangentialDistance(const Point & p) const;

  /// Get the normal component of the distance between the given point \p p and the path's position at time \p t
  Real normalDistance(Real t, const Point & p) const;

  /// Get the normal component of the distance between the given point \p p and the path's current position
  Real normalDistance(const Point & p) const;

protected:
  /// Verbose?
  const bool _verbose;

private:
  /// The absolute epsilon to use in finite-differencing
  const Real _abs_epsilon;
  /// The relative epsilon to use in finite-differencing
  const Real _rel_epsilon;

  /// Path update interval
  const Real _interval;
  /// Last time the path front was updated
  Real & _last_updated;

  /// Whether to apply direction/velocity smoothing
  const bool _smooth;
  /// The smoothing window
  const Real _smooth_window;
  /// The number of smoothing points
  const unsigned int _smooth_points;

  /// The current position
  Point _current_position;
  /// The current velocity
  RealVectorValue _current_velocity;
  /// The current direction
  RealVectorValue _current_direction;

  /// The previous position
  Point & _previous_position;
  /// The current velocity
  RealVectorValue & _previous_velocity;
  /// The current direction
  RealVectorValue & _previous_direction;
};
