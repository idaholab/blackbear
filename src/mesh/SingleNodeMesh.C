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

#include "SingleNodeMesh.h"

#include "libmesh/elem.h"
#include "libmesh/mesh_base.h"

registerMooseObject("BlackBearApp", SingleNodeMesh);

InputParameters
SingleNodeMesh::validParams()
{
  InputParameters params = MooseMesh::validParams();
  params.addClassDescription("A mesh object containing a single NODEELEM.");
  params.addParam<RealVectorValue>(
      "position", RealVectorValue(0.0, 0.0, 0.0), "Coordinates of the single node.");
  return params;
}

SingleNodeMesh::SingleNodeMesh(const InputParameters & parameters)
  : MooseMesh(parameters), _position(getParam<RealVectorValue>("position"))
{
}

std::unique_ptr<MooseMesh>
SingleNodeMesh::safeClone() const
{
  return std::make_unique<SingleNodeMesh>(*this);
}

void
SingleNodeMesh::buildMesh()
{
  auto & mesh = getMesh(); // or the equivalent underlying libMesh accessor in your app

  mesh.clear();
  mesh.set_spatial_dimension(3);

  Node * node = mesh.add_point(Point(_position(0), _position(1), _position(2)), 0);
  Elem * elem = mesh.add_elem(Elem::build(NODEELEM).release());
  elem->set_node(0, node);

  mesh.prepare_for_use();

  // Force the FE-facing dimension you want after preparation
  mesh.set_mesh_dimension(1);
}
