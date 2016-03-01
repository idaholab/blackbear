/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                        Grizzly                               */
/*                                                              */
/*           (c) 2015 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef SETREACTIONNETWORKACTION_H
#define SETREACTIONNETWORKACTION_H

// MOOSE includes
#include "Action.h"

// libMesh includes
#include "libmesh/fe_type.h"

class SetReactionNetworkAction;

template<>
InputParameters validParams<SetReactionNetworkAction>();


class SetReactionNetworkAction : public Action
{
public:
  SetReactionNetworkAction(InputParameters params);

  virtual void act();

protected:
  const FEType _fe_type;
};

#endif // SETREACTIONNETWORKACTION_H
