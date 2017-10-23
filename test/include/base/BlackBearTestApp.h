#ifndef BLACK_BEARTESTAPP_H
#define BLACK_BEARTESTAPP_H

#include "MooseApp.h"

class BlackBearTestApp;

template <>
InputParameters validParams<BlackBearTestApp>();

class BlackBearTestApp : public MooseApp
{
public:
  BlackBearTestApp(InputParameters parameters);
  virtual ~BlackBearTestApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* BLACK_BEARTESTAPP_H */
