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

#ifndef BLACKBEARSYNTAX_H
#define BLACKBEARSYNTAX_H

#include "Factory.h"
#include "Syntax.h"
#include "ActionFactory.h"

namespace BlackBear
{
void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
}

#endif // BLACKBEARSYNTAX_H
