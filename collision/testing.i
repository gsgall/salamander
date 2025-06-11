# the number of computational particles to be put in each element
particles_per_element = 1000
# the analytic solution for the charge density
# set by cli args for each case since it is needed by all
# tests but changes based on the dimension of the problem
number_density = 1

[Problem]
  solve = false
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 2
    ny = 2
    xmax = 1
    ymax = 1
  []
  allow_renumbering = false
[]

[Distributions]
  [zero]
    type = Constant
    value = 0
  []

  [uniform]
    type = Uniform
    lower_bound = -0.5
    upper_bound = 0.5
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
    # type = TestStationaryStepper
  []

  [initializer]
    type = PerElementParticleInitializer
    mass = 1
    charge = 1
    number_density = ${number_density}
    particles_per_element = ${particles_per_element}
    velocity_distributions = 'uniform uniform zero'
  []

  [study]
    type = CollisionalPICStudy
    stepper = stepper
    initializers = initializer
    cross_section = 1
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = TIMESTEP_BEGIN
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[RayBCs]
  [walls]
    type = ReflectRayBC
    boundary = 'left right top bottom'
  []
[]

[VectorPostprocessors]

  [particles]
    type = ParticleDataVectorPostprocessor
    study = study
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 100
[]

[Outputs]
  exodus = false
  csv = true
[]
