# AverageExtension

!syntax description /Postprocessors/AverageExtension

## Description

This `PostProcessor` computes the average extension between two sets of points specified in two arrays defined with the `first_point` and `last_point` parameters. The point positions account for the current displacements, so this distance will evolve over time for a deforming body. If the separation between two specific points is desired, single points rather than a arrays of points can be specified.

This `PostProcessor` computes average extension between a set of points specified in two array `first_point` and `last_point`. The extensions between the points with matching positions in these arrays are computed, and then their average is computed. The lengths of these sets of points must be the same.

## Example Input File Syntax

!listing test/tests/AverageExtension/AverageExtension.i block=Postprocessors/avg_extension

!syntax parameters /Postprocessors/AverageExtension

!syntax inputs /Postprocessors/AverageExtension

!syntax children /Postprocessors/AverageExtension

!bibtex bibliography
