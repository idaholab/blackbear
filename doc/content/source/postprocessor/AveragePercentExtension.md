# AveragePercentExtension

!syntax description /Postprocessors/AveragePercentExtension

## Description

This `PostProcessor` computes the average percentage extension between two sets of points specified in two arrays defined with the `first_point` and `last_point` parameters. The point positions account for the current displacements, so this distance will evolve over time for a deforming body. If the separation between two specific points is desired, single points rather than a arrays of points can be specified.

This `PostProcessor` computes average percentage extension between a set of points specified in two array `first_point` and `last_point`.

## Example Input File Syntax

!listing test/tests/AveragePercentExtension/AveragePercentExtension.i block=Postprocessors/avg_extension

!syntax parameters /Postprocessors/AveragePercentExtension

!syntax inputs /Postprocessors/AveragePercentExtension

!syntax children /Postprocessors/AveragePercentExtension

!bibtex bibliography
