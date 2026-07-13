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
    nx = 3
    xmin = 0
    xmax = 10
  []
  allow_renumbering = false
[]


[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []
  [velocity_initializer]
    type = ConstantVelocityInitializer
    velocities = '6 0 0'
  []
  [particle_initializer]
    type = TestPlacedParticleInitializer
    start_points = '5 0 0'
    velocity_initializer = 'velocity_initializer'
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
  [bc]
    type = ChangeRayMaxDistanceRayBC
    max_distance = 2
    boundary = 'left right'
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
  num_steps = 1
[]

[Outputs]
  exodus = false
  [csv]
    type = CSV
    start_step = 1
  []
[]
