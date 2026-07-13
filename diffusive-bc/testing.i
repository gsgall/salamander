particles_per_element = 1000
m = 1e-20

[Problem]
  solve = false
[]

[GlobalParams]
  seed = 9812435
  study = study
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 11
    xmax = 100
  []
  allow_renumbering = false
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []
  [velocity_initializer]
    type = ConstantVelocityInitializer
    velocities = '100 0 0'
  []
  [particle_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = ${particles_per_element}
    number_density = 10
    charge = 0
    mass = ${m}
    velocity_initializer = velocity_initializer
  []
  [study]
    type = PICStudy
    stepper = stepper
    particle_initializers = particle_initializer
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
  [diffusive]
    type = DiffusiveReflectParticleBC
    temperature = 273.15
    boundary = 'right'
  []
  [specular]
    type = ReflectParticleBC
    boundary = 'left'
  []
[]

[VectorPostprocessors]
  [particles]
    type = ParticleDataVectorPostprocessor
    study = study
  []
  [distribution]
    type = CustomHistogramVectorPostprocessor
    num_bins = 75
    vpp = particles
    column_names = 'v_x v_y v_z'
    lower_bound = -3
    upper_bound = 3
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 1
[]

[Outputs]
  exodus = false
  [csv]
    type = CSV
    #time_step_interval = 5
    #hide = particles
  []
[]
