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

#include "ElementSubdomainModifier.h"
#include "SpatioTemporalPath.h"
#include "SpatioTemporalPathInterface.h"

class SpatioTemporalPathElementSubdomainModifier : public ElementSubdomainModifier,
                                                   public SpatioTemporalPathInterface
{
public:
  static InputParameters validParams();

  SpatioTemporalPathElementSubdomainModifier(const InputParameters & parameters);

protected:
  virtual SubdomainID computeSubdomainID() override;

  /// The path
  const SpatioTemporalPath & _path;

  /// Target subdomain ID
  const SubdomainID _subdomain_id;

  /// Radius
  const Real _r;
};
