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

#include "SingleNodeMeshGenerator.h"

#include "CastUniquePointer.h"

#include "libmesh/replicated_mesh.h"
#include "libmesh/elem.h"

registerMooseObject("BlackBearApp", SingleNodeMeshGenerator);

InputParameters
SingleNodeMeshGenerator::validParams()
{
  InputParameters params = MeshGenerator::validParams();
  params.addClassDescription(
      "Generates a mesh with a single node and a single NODEELEM element, suitable for "
      "zero-dimensional (point) nodal simulations such as cluster dynamics.  A NODEELEM "
      "is libMesh's zero-dimensional point element: it contains exactly one node and has "
      "no volume or quadrature points.");
  params.addParam<RealVectorValue>(
      "position", RealVectorValue(0.0, 0.0, 0.0), "Coordinates of the single node.");
  return params;
}

SingleNodeMeshGenerator::SingleNodeMeshGenerator(const InputParameters & parameters)
  : MeshGenerator(parameters), _position(getParam<RealVectorValue>("position"))
{
}

std::unique_ptr<MeshBase>
SingleNodeMeshGenerator::generate()
{
  auto mesh = buildMeshBaseObject();
  mesh->set_spatial_dimension(3);

  // A NODEELEM is a zero-dimensional libMesh "point element" - it is exactly
  // one node with no geometry or sides.  It is required for two reasons:
  //   1. prepare_for_use() removes orphan nodes (nodes not attached to any
  //      element), so a bare add_point() call would leave the mesh empty.
  //   2. PETSc's DMlibMesh requires at least one mesh block, which is only
  //      defined if the mesh contains at least one element.
  Node * node = mesh->add_point(Point(_position(0), _position(1), _position(2)), 0);
  Elem * elem = mesh->add_elem(Elem::build(NODEELEM).release());
  elem->set_node(0, node);

  // prepare_for_use() calls cache_elem_data() which rebuilds _elem_dims from
  // the actual elements.  NODEELEM has dim() == 0, so mesh_dimension() returns
  // 0 afterwards which causes errors.  Overriding to 1 after prepare_for_use() fixes
  // this: the loop iterates over dim = 0 only, which is a valid FE dimension.
  mesh->prepare_for_use();
  mesh->set_mesh_dimension(1);

  return dynamic_pointer_cast<MeshBase>(mesh);
}
