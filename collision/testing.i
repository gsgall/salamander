# the number of computational particles to be put in each element
particles_per_element = 1e3
# the analytic solution for the charge density
# set by cli args for each case since it is needed by all
# tests but changes based on the dimension of the problem
number_density = 1

m = 6.6e-26
k_B = 1.380649e-23
T_x = 373
T_yz = 273
sigma_x = '${fparse sqrt(k_B * T_x / m)}'
sigma_yz = '${fparse sqrt(k_B * T_yz / m)}'

[Problem]
  solve = false
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 90
    ny = 90
    xmax = 1e6
    ymax = 1e6
  []
  allow_renumbering = false
[]

[Distributions]
  [v_x]
    type = Normal
    mean = 0
    standard_deviation = '${sigma_x}'
  []
  [v_yz]
    type = Normal
    mean = 0
    standard_deviation = '${sigma_yz}'
  []
[]

[UserObjects]
  [stepper]
    # type = TestSimpleStepper
    type = TestStationaryStepper
  []

  [initializer]
    type = PerElementParticleInitializer
    mass = ${m}
    charge = 1
    number_density = ${number_density}
    particles_per_element = ${particles_per_element}
    velocity_distributions = 'v_x v_yz v_yz'
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
  [velocities]
    type = ParticleVelocityVectorPostprocessor
    study = study
  []
  [distribution]
    type = HistogramVectorPostprocessor
    num_bins = 30
    vpp = velocities
  []
[]

[Executioner]
  type = Transient
  dt = 1e-2
  # dt = 1e-10
  # num_steps = 1
  num_steps = 1000
[]

[Outputs]
  exodus = false
  [csv]
    type = CSV
    show = 'distribution particle_count'
  []
  interval = 10
[]
