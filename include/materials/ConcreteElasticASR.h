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
