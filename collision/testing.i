# the number of computational particles to be put in each element
particles_per_element = 5e4
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
    nx = 1
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
    # type = TestSimpleStepper
    type = TestStationaryStepper
  []

  [initializer]
    type = PerElementParticleInitializer
    mass = 1
    charge = 1
    number_density = ${number_density}
    particles_per_element = ${particles_per_element}
    velocity_distributions = 'uniform uniform uniform'
  []

  [study]
    type = CollisionalPICStudy
    stepper = stepper
    initializers = initializer
    cross_section = 1
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = TIMESTEP_BEGIN
    tolerate_failure = true
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

[Postprocessors]

  [particle_count]
    type = RayTracingStudyResult
    result = 'total_rays_started'
    study = study
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
  dt = 1e-2
  # dt = 1e-10
  # num_steps = 1
  num_steps = 101
[]

[Outputs]
  exodus = false
  csv = true
  interval = 100
[]
