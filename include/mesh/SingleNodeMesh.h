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

#include "MooseMesh.h"

class SingleNodeMesh : public MooseMesh
{
public:
  static InputParameters validParams();
  SingleNodeMesh(const InputParameters & parameters);

  virtual std::unique_ptr<MooseMesh> safeClone() const override;
  virtual void buildMesh() override;

protected:
  RealVectorValue _position;
};
