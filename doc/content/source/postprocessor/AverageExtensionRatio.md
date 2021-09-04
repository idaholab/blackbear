# AverageExtensionRatio

!syntax description /Postprocessors/AverageExtensionRatio

## Description

This `PostProcessor` computes the average extension ratio between two sets of points specified in two arrays defined with the `first_point` and `last_point` parameters. The point positions account for the current displacements, so this distance will evolve over time for a deforming body. If the separation between two specific points is desired, single points rather than a arrays of points can be specified.

This `PostProcessor` computes average extension ratio between a set of points specified in two array `first_point` and `last_point`. The extensions between the points with matching positions in these arrays are computed, and then their average is computed. The lengths of these sets of points must be the same.

## Example Input File Syntax

!listing test/tests/AverageExtensionRatio/AverageExtensionRatio.i block=Postprocessors/avg_extension_raio

!syntax parameters /Postprocessors/AverageExtensionRatio

!syntax inputs /Postprocessors/AverageExtensionRatio

!syntax children /Postprocessors/AverageExtensionRatio

!bibtex bibliography
