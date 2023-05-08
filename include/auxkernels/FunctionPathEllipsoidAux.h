// Fernando Valiente Dies & Nicolò Grilli
// University of Sydney & University of Bristol
// 29-11-2022

#pragma once

#include "AuxKernel.h"
//#include "VelocityEllipsoidHeatSource.h"

class Function;

/**
 * This AuxKernel increases an AuxVariable from 0 to 1 if the qp is inside
 * an ellipsoid that is moving according to a path provided through a function.
 * It can be applied to a level set variable
 * to simulate the material deposition during wire arc additive manufacturing
 * together with ActDeactElementsCoupled.
 */
class FunctionPathEllipsoidAux : public AuxKernel
{
public:
  static InputParameters validParams();

  FunctionPathEllipsoidAux(const InputParameters & parameters);

protected:
  void initQpStatefulProperties();
  virtual Real computeValue() override;
  
  // The default is 0 to 1, but in general the level set will be transformed from
  // _low_level_set_var to _high_level_set_var
  const Real _low_level_set_var;
  const Real _high_level_set_var;
  
  /// transverse ellipsoid axe
  const Real _rx;
  /// depth ellipsoid axe
  const Real _ry;
  /// longitudinal ellipsoid axe
  const Real _rz;
  
  /// Scanning speed vector
  const VectorPostprocessorValue & _velocity;
  
  /// Initial values of the coordinates of the heat source
  /// Every time the postprocessor condition is satisfied, 
  /// the heat source is moved to the next set of coordinates
  const VectorPostprocessorValue & _init_x_coords;
  const VectorPostprocessorValue & _init_y_coords;
  const VectorPostprocessorValue & _init_z_coords;
  
  /// Postprocess with temperature value
  /// it provides the condition based on which the heat source
  /// is moved to the next set of initial coordinates
  const PostprocessorValue & _temperature_pp;
  
  /// variables to store the coordinates 
  /// of the center of the heat source
  Real _x_coord;
  Real _y_coord;
  Real _z_coord;
  Real _vel_x;
  
  /// Total time during one scan
  /// After this time the laser is switched off
  const VectorPostprocessorValue & _single_scan_length;
  
  /// When the temperature provided by the postprocessor decreases
  /// below this threshold, the heat source is moved to the next
  /// set of coordinates
  const Real _threshold_temperature;
  
  // Threshold value of the ellipsoid function
  // that activates the level set.
  const Real _level_set_activation_threshold;

    /// _t_scan tracks the simulation time at which a new
  /// scan begins after the condition based on the postprocessor
  /// changes the coordinates of the heat source
  /// imported from a VelocityEllipsoidHeatSource material object
  const MaterialProperty<Real> & _t_scan;
  
  /// Material deposition track index
  /// imported from a VelocityEllipsoidHeatSource material object
  const MaterialProperty<int> & _n_track;

};
