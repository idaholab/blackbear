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

#include "MeshGenerator.h"

/**
 * Generates a mesh with a single node and a single NODEELEM element,
 * suitable for zero-dimensional (point) nodal simulations such as cluster
 * dynamics.  A NODEELEM is libMesh's zero-dimensional point element: it
 * contains exactly one node and has no volume or quadrature points.
 */
class SingleNodeMeshGenerator : public MeshGenerator
{
public:
  static InputParameters validParams();
  SingleNodeMeshGenerator(const InputParameters & parameters);

  std::unique_ptr<MeshBase> generate() override;

protected:
  /// Coordinates of the single node
  const RealVectorValue _position;
};
