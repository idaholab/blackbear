/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

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
