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

#include "Action.h"

class SetReactionNetworkAction;

template<>
InputParameters validParams<SetReactionNetworkAction>();


class SetReactionNetworkAction : public Action
{
public:
  SetReactionNetworkAction(const std::string & name, InputParameters params);

  virtual void act();

protected:
  const std::string _order;
  const std::string _family;
};

#endif // SETREACTIONNETWORKACTION_H
