# the number of computational particles to be put in each element
particles_per_element = 10
# the analytic solution for the charge density
# set by cli args for each case since it is needed by all
# tests but changes based on the dimension of the problem
charge_density = 0

[GlobalParams]
  particles_per_element = ${particles_per_element}
  seed = 0
[]

[Problem]
  extra_tag_vectors = dump_value
[]

[Variables]
  [phi]
  []

  [n]
  []
[]

[Kernels]
  [poissons]
    type = ADMatDiffusion
    diffusivity = 1
    variable = phi
  []

  [projection]
    type = ProjectionKernel
    variable = n
  []
[]


[AuxVariables]
  [dump_value]
  []
  [Ex]
    order = CONSTANT
    family = MONOMIAL
  []
  [Ey]
    order = CONSTANT
    family = MONOMIAL
  []
  [Ez]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [dump_value]
    type = TagVectorAux
    variable = dump_value
    vector_tag = dump_value
    v = phi
  []
  [x_grad]
    type = NegativeVariableGradientComponent
    variable = Ex
    gradient_variable = phi
    component = 0
  []
  [y_grad]
    type = NegativeVariableGradientComponent
    variable = Ey
    gradient_variable = phi
    component = 1
  []
  [z_grad]
    type = NegativeVariableGradientComponent
    variable = Ez
    gradient_variable = phi
    component = 2
  []
[]


[UserObjects]
  [velocity_initializer]
    type = ConstantVelocityInitializer
    velocities = '0 0 0'
  []
  [initializer]
    type = PerElementParticleInitializer
    mass = 1
    charge = 1
    number_density = ${charge_density}
    velocity_initializer = 'velocity_initializer'
  []

  [stepper]
    type = TestSimpleStepper
  []

  [study]
    type = TestInitializedPICStudy
    initializers = initializer
    stepper = stepper
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on=TIMESTEP_BEGIN
  []

  [potential_accumulator]
    type = ChargeDensityAccumulator
    study = study
    variable = phi
    extra_vector_tags = dump_value
  []

  [density_accumulator]
    type = ChargeDensityAccumulator
    study = study
    variable = n
  []
[]


[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[Functions]
  [charge_density]
    type = ParsedFunction
    expression = ${charge_density}
  []
[]

[Functions]
  [potential]
    type = ParsedFunction
    expression = 'x * (1 - x) + y * (1 - y) + z * (1 - z)'
  []
  [Ex_analytic]
    type = ParsedGradFunction
    expression = '2 * x - 1'
  []
  [Ey_analytic]
    type = ParsedGradFunction
    expression = '2 * y - 1'
  []
  [Ez_analytic]
    type = ParsedGradFunction
    expression = '2 * z - 1'
  []
[]


[Postprocessors]
  [potential_l2_error]
    type = ElementL2Error
    variable = phi
    function = potential
  []

  [potential_l2]
    type = ElementL2Norm
    variable = phi
  []

  [Ex_l2_error]
    type = ElementL2Error
    variable = Ex
    function = Ex_analytic
  []

  [Ex_l2]
    type = ElementL2Norm
    variable = Ex
  []

  [Ey_l2_error]
    type = ElementL2Error
    variable = Ey
    function = Ey_analytic
  []

  [Ey_l2]
    type = ElementL2Norm
    variable = Ey
  []

  [Ez_l2_error]
    type = ElementL2Error
    variable = Ez
    function = Ez_analytic
  []

  [Ez_l2]
    type = ElementL2Norm
    variable = Ez
  []

  [density_l2_error]
    type = ElementL2Error
    variable = n
    function = charge_density
  []

  [density_l2]
    type = ElementL2Norm
    variable = n
  []

  [particles_per_element]
    type = ConstantPostprocessor
    value = ${particles_per_element}
  []

  [h]
    type = AverageElementSize
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver'
  petsc_options_value = 'lu NONZERO 1.e-9 superlu_dists'
  line_search = 'none'
  nl_max_its = 15
  l_max_its = 300
  scheme = 'bdf2'
  automatic_scaling = true
  compute_scaling_once = false
  dt = 1
  num_steps = 1
[]

[Outputs]
  exodus = true
  [csv]
    type = CSV
    execute_on = TIMESTEP_END
  []
[]
