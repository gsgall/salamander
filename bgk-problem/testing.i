# the number of computational particles to be put in each element
particles_per_element = 20
# the analytic solution for the charge density
# set by cli args for each case since it is needed by all
# tests but changes based on the dimension of the problem
number_density = 1

m = 6.6e-26
T_x = 373
T_yz = 273

[Problem]
  solve = false
[]

[GlobalParams]
  seed = 0
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 10
    xmax = 1e3
    ymax = 1e3
  []
  allow_renumbering = false
[]

[Distributions]
  [v_x]
    type = Maxwellian
    mass = ${m}
    temperature = ${T_x}
  []
  [v_yz]
    type = Maxwellian
    mass = ${m}
    temperature = ${T_yz}
  []
[]

[UserObjects]
  [stepper]
    # type = TestSimpleStepper
    type = TestStationaryStepper
  []
  [velocity_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_x v_yz v_yz'
  []
  [particle_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = ${particles_per_element}
    # number_density = 1
    number_density = ${number_density}
    charge = 0
    # mass = 1
    mass = ${m}
    # velocity_initializer = constant_initializer
    velocity_initializer = velocity_initializer
  []
  [maxwell]
    type = MaxwellCollision
    reactants = 'A A'
    products = 'A A'
    study = study
    reference_value = 1
  []
  [collider]
    type = DSMCCollider
    study = study
    collision_objects = 'maxwell'
  []
  [study]
    type = CollisionalPICStudy
    # type = CollisionlessPICStudy
    stepper = stepper
    particle_initializers = particle_initializer
    collider = collider
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
    ray_kernel_coverage_check = false
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[RayBCs]
  [walls]
    type = ReflectParticleBC
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
    num_bins = 250
    vpp = velocities
  []
[]

[Executioner]
  type = Transient
  dt = 1
  # dt = 1e-10
  # num_steps = 2
  num_steps = 100
[]

[Outputs]
  exodus = false
  [csv]
    type = CSV
    show = 'distribution particle_count'
  []
  # interval = 5
[]
