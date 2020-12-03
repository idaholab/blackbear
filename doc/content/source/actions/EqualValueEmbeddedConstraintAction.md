<!-- MOOSE Documentation Stub: Remove this when content is added. -->

# EqualValueEmbeddedConstraintAction

!syntax description /Actions/EqualValueEmbeddedConstraintAction

This constraint action apply EqualValueEmbeddedConstraint upon overlapping portion of two blocks, primary and secondary blocks, as defined in EqualValueEmbeddedConstraint.C. This constraint iterate over the list of primary and secondary blocks for a given list of displacement components. Thus, action will apply EqualValueEmbeddedConstraint on all combination of primary block, secondary block, and displacement component. This action helps in reducing the length of input file as user do not need to apply constraint for each combination of primary and secondary blocks and displacement components.

!syntax parameters /Actions/EqualValueEmbeddedConstraint

!syntax inputs /Actions/EqualValueEmbeddedConstraint
