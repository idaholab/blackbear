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

#include "SpatioTemporalPathElementSubdomainModifier.h"

registerMooseObject("BlackBearApp", SpatioTemporalPathElementSubdomainModifier);

InputParameters
SpatioTemporalPathElementSubdomainModifier::validParams()
{
  InputParameters params = ElementSubdomainModifier::validParams();
  params.addClassDescription("Modify subdomain of elements when the element is within the "
                             "neighborhood of the path's current position.");
  params.addRequiredParam<SubdomainName>(
      "target_subdomain", "The subdomain name/ID to set when the path goes through an element");
  params.addRequiredParam<std::string>("path", "The name of the spatio-temporal path object.");
  params.addRequiredParam<Real>("radius",
                                "The element subdomain is changed to the target subdomain if its "
                                "centroid is within the radius of the current path front.");
  return params;
}

SpatioTemporalPathElementSubdomainModifier::SpatioTemporalPathElementSubdomainModifier(
    const InputParameters & params)
  : ElementSubdomainModifier(params),
    SpatioTemporalPathInterface(this),
    _path(getSpatioTemporalPath("path")),
    _subdomain_id(_mesh.getSubdomainID(getParam<SubdomainName>("target_subdomain"))),
    _r(getParam<Real>("radius"))
{
}

SubdomainID
SpatioTemporalPathElementSubdomainModifier::computeSubdomainID()
{
  if ((_current_elem->centroid() - _path.position()).norm_sq() < _r * _r)
    return _subdomain_id;

  return _current_elem->subdomain_id();
}
