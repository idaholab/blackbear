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

#include "NEMLStressBase.h"

class NEMLStress : public NEMLStressBase
{
public:
  static InputParameters validParams();
  NEMLStress(const InputParameters & parameters);

protected:
  /// File name of the NEML XML database
  FileName _fname;
  /// Model name in NEML XML database
  std::string _mname;
};
