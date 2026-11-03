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

#include "AddVariableAction.h"
#include "ClusterGroupingLayout.h"

class GroupedClusterVariableAction : public AddVariableAction
{
public:
  static InputParameters validParams();

  GroupedClusterVariableAction(const InputParameters & params);

  virtual void act() override;
};
