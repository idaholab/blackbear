[Constraints/EqualValueEmbeddedConstraint]
  [batch]
    primary = '1'
    secondary = '2 3'
    variable = 'disp_x disp_y'
    penalty = 1e12
    formulation = penalty
  []
  # Split into a second batch to test that two instances of this
  # block can be used together. This should behave the same as
  # listing disp_x, disp_y, and disp_z in a single block.
  [batch2]
    primary = '1'
    secondary = '2 3'
    variable = 'disp_z'
    penalty = 1e12
    formulation = penalty
  []
[]
