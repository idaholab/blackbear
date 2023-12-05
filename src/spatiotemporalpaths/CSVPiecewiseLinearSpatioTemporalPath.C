/****************************************************************************/
/*                        DO NOT MODIFY THIS HEADER                         */
/*                                                                          */
/* MALAMUTE: MOOSE Application Library for Advanced Manufacturing UTilitiEs */
/*                                                                          */
/*           Copyright 2021 - 2023, Battelle Energy Alliance, LLC           */
/*                           ALL RIGHTS RESERVED                            */
/****************************************************************************/

#include "CSVPiecewiseLinearSpatioTemporalPath.h"
#include "DelimitedFileReader.h"

registerMooseObject("MalamuteApp", CSVPiecewiseLinearSpatioTemporalPath);

InputParameters
CSVPiecewiseLinearSpatioTemporalPath::validParams()
{
  InputParameters params = PiecewiseLinearSpatioTemporalPathBase::validParams();
  params.addClassDescription("A piecewise linear spatiotemporal path from a csv file.");
  params.addRequiredParam<FileName>(
      "file", "CSV file containing information about the spatio-temporal path");
  MooseEnum header_flag("OFF ON AUTO", "AUTO");
  params.addParam<MooseEnum>("file_header",
                             header_flag,
                             "Set the header flag. ON: use the first row has header, OFF: assumes "
                             "no header, AUTO: attempt to determine if a header exists.");
  MooseEnum format_flag("COLUMNS ROWS", "COLUMNS");
  params.addParam<MooseEnum>("file_format", format_flag, "Set the file format (rows vs. columns).");
  params.addParam<bool>("file_ignore_empty_lines", true, "Ignore empty lines in the csv file.");
  params.addParam<std::string>(
      "file_comment", "", "Set the comment character, by default no comment character is used.");
  params.addParam<std::string>("t", "t", "Column name for the spatio-temporal path's time");
  params.addParam<std::string>("x", "x", "Column name for the spatio-temporal path's x-coordinate");
  params.addParam<std::string>("y", "y", "Column name for the spatio-temporal path's y-coordinate");
  params.addParam<std::string>("z", "z", "Column name for the spatio-temporal path's z-coordinate");
  return params;
}

CSVPiecewiseLinearSpatioTemporalPath::CSVPiecewiseLinearSpatioTemporalPath(
    const InputParameters & params)
  : PiecewiseLinearSpatioTemporalPathBase(params)
{
  using Format = MooseUtils::DelimitedFileReader::FormatFlag;
  using Header = MooseUtils::DelimitedFileReader::HeaderFlag;

  auto csv_reader = MooseUtils::DelimitedFileReader(getParam<FileName>("file"));
  csv_reader.setHeaderFlag(getParam<MooseEnum>("file_header").getEnum<Header>());
  csv_reader.setFormatFlag(getParam<MooseEnum>("file_format").getEnum<Format>());
  csv_reader.setIgnoreEmptyLines(getParam<bool>("file_ignore_empty_lines"));
  csv_reader.setComment(getParam<std::string>("file_comment"));
  csv_reader.read();

  // times
  _times = csv_reader.getData(getParam<std::string>("t"));

  // coords
  const auto & x = csv_reader.getData(getParam<std::string>("x"));
  const auto & y = csv_reader.getData(getParam<std::string>("y"));
  const auto & z = csv_reader.getData(getParam<std::string>("z"));
  setCoords(x, y, z);

  validate();
}
