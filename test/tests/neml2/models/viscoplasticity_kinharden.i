[Solvers]
  [newton]
    type = NewtonNonlinearSolver
  []
[]

[Models]
  [kinharden]
    type = LinearKinematicHardening
    hardening_modulus = 1000
  []
  [mandel_stress]
    type = IsotropicMandelStress
  []
  [overstress]
    type = OverStress
  []
  [vonmises]
    type = SR2Invariant
    invariant_type = 'VONMISES'
    tensor = 'state/internal/O'
    invariant = 'state/internal/sm'
  []
  [yield]
    type = YieldFunction
    yield_stress = 5
  []
  [flow]
    type = ComposedModel
    models = 'overstress vonmises yield'
  []
  [normality]
    type = Normality
    model = 'flow'
    function = 'state/internal/fp'
    from = 'state/internal/M state/internal/X'
    to = 'state/internal/NM state/internal/NX'
  []
  [flow_rate]
    type = PerzynaPlasticFlowRate
    reference_stress = 100
    exponent = 2
  []
  [Kprate]
    type = AssociativeKinematicPlasticHardening
  []
  [Eprate]
    type = AssociativePlasticFlow
  []
  [Erate]
    type = SR2ForceRate
    force = 'E'
  []
  [Eerate]
    type = ElasticStrain
    rate_form = true
  []
  [elasticity]
    type = LinearIsotropicElasticity
    youngs_modulus = 1e5
    poisson_ratio = 0.3
    rate_form = true
  []
  [integrate_Kp]
    type = SR2BackwardEulerTimeIntegration
    variable = 'internal/Kp'
  []
  [integrate_stress]
    type = SR2BackwardEulerTimeIntegration
    variable = 'S'
  []
  [implicit_rate]
    type = ComposedModel
    models = 'kinharden mandel_stress overstress vonmises yield normality flow_rate Eprate Kprate Erate Eerate elasticity integrate_stress integrate_Kp'
  []
  [model]
    type = ImplicitUpdate
    implicit_model = 'implicit_rate'
    solver = 'newton'
  []
[]
