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

#include "SpatioTemporalPath.h"
#include "DelimitedFileReader.h"

registerMooseObject("BlackBearApp", SpatioTemporalPath);

InputParameters
SpatioTemporalPath::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.addClassDescription("A piecewise linear spatio-temporal path, given either as vertex "
                             "lists (t, x, y, z) or as a CSV file with columns t, x, y, z.");

  params.addParam<std::vector<Real>>("t", "Times of the path vertices");
  params.addParam<std::vector<Real>>("x", "x-coordinates of the path vertices");
  params.addParam<std::vector<Real>>("y", "y-coordinates of the path vertices");
  params.addParam<std::vector<Real>>("z", "z-coordinates of the path vertices");
  params.addParamNamesToGroup("t x y z", "Vertices from the input file");

  params.addParam<FileName>("file",
                            "CSV file with columns named 't' and any of 'x', 'y', 'z'. "
                            "Use this instead of the t, x, y, z parameters.");
  params.addParamNamesToGroup("file", "Vertices from a CSV file");

  // The path is updated in initialSetup()/timestepSetup(), not by execute()
  params.set<ExecFlagEnum>("execute_on") = EXEC_NONE;
  params.suppressParameter<ExecFlagEnum>("execute_on");
  return params;
}

SpatioTemporalPath::SpatioTemporalPath(const InputParameters & params)
  : GeneralUserObject(params)
{
  const bool from_file = isParamValid("file");
  const bool from_lists = isParamValid("t");
  if (from_file == from_lists)
    mooseError("Provide either 'file' or 't' (with at least one of 'x', 'y', 'z'), but not both.");

  std::vector<Real> t;
  std::array<std::vector<Real>, 3> c;
  const std::array<std::string, 3> names = {"x", "y", "z"};

  if (from_file)
  {
    for (const auto & n : {"t", "x", "y", "z"})
      if (isParamValid(n))
        paramError(n, "Cannot be combined with 'file'.");

    MooseUtils::DelimitedFileReader reader(getParam<FileName>("file"));
    reader.setFormatFlag(MooseUtils::DelimitedFileReader::FormatFlag::COLUMNS);
    reader.read();
    const auto & cols = reader.getNames();
    auto has = [&cols](const std::string & n)
    { return std::find(cols.begin(), cols.end(), n) != cols.end(); };
    if (!has("t"))
      paramError("file", "The CSV file must have a column named 't'.");
    t = reader.getData("t");
    for (const auto i : make_range(3))
      if (has(names[i]))
        c[i] = reader.getData(names[i]);
  }
  else
  {
    t = getParam<std::vector<Real>>("t");
    for (const auto i : make_range(3))
      if (isParamValid(names[i]))
        c[i] = getParam<std::vector<Real>>(names[i]);
  }

  if (t.size() < 2)
    mooseError("The path needs at least two vertices.");
  if (c[0].empty() && c[1].empty() && c[2].empty())
    mooseError("At least one of the x, y, z coordinates must be given.");

  for (const auto i : make_range(3))
  {
    // An omitted coordinate is zero along the whole path
    if (c[i].empty())
      c[i].assign(t.size(), 0.0);
    else if (c[i].size() != t.size())
      mooseError("'", names[i], "' has ", c[i].size(), " values but 't' has ", t.size(), ".");

    // LinearInterpolation checks that t is strictly increasing. Outside the time range the
    // position is held at the first/last vertex.
    try
    {
      _coord[i].setData(t, c[i]);
    }
    catch (std::domain_error & e)
    {
      mooseError("Invalid path times: ", e.what());
    }
  }
}

void
SpatioTemporalPath::update()
{
  _position = position(_t);
  _direction = direction(_t);
}

Point
SpatioTemporalPath::position(Real t) const
{
  return Point(_coord[0].sample(t), _coord[1].sample(t), _coord[2].sample(t));
}

RealVectorValue
SpatioTemporalPath::direction(Real t) const
{
  // Exact slope of the active segment. A corner gives a jump in direction, which is intended.
  const RealVectorValue v(
      _coord[0].sampleDerivative(t), _coord[1].sampleDerivative(t), _coord[2].sampleDerivative(t));
  return v.norm() > 0 ? v.unit() : RealVectorValue(0, 0, 0);
}

Real
SpatioTemporalPath::tangentialDistance(const Point & p) const
{
  return (p - _position) * _direction;
}

Real
SpatioTemporalPath::normalDistance(const Point & p) const
{
  const auto dp = p - _position;
  return (dp - (dp * _direction) * _direction).norm();
}
