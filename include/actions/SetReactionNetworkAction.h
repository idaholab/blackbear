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
