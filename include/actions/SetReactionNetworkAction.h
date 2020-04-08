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

// MOOSE includes
#include "Action.h"

// libMesh includes
#include "libmesh/fe_type.h"



class SetReactionNetworkAction : public Action
{
public:
  static InputParameters validParams();
  SetReactionNetworkAction(InputParameters params);

  virtual void act();

protected:
  const FEType _fe_type;
};
