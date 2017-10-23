#ifndef BLACK_BEARAPP_H
#define BLACK_BEARAPP_H

#include "MooseApp.h"

class BlackBearApp;

template <>
InputParameters validParams<BlackBearApp>();

class BlackBearApp : public MooseApp
{
public:
  BlackBearApp(InputParameters parameters);
  virtual ~BlackBearApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void registerObjectDepends(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
  static void associateSyntaxDepends(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* BLACK_BEARAPP_H */
