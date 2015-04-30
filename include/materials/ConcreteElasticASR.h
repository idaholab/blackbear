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

#ifndef CONCRETEELASTICASR_H
#define CONCRETEELASTICASR_H

#include "SolidModel.h"

class ConcreteElasticASR : public SolidModel
{
public:
  ConcreteElasticASR( const std::string & name,
           InputParameters parameters );
  virtual ~ConcreteElasticASR();

protected:

};

template<>
InputParameters validParams<ConcreteElasticASR>();


#endif
