[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 1
  nx = 3
  xmax = 1
[]

[Problem]
  solve = false
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [velocity_initializer]
    type = ConstantVelocityInitializer
    velocities = '1 0 0'
  []

  [reset_initializer]
    type = ConstantVelocityInitializer
    velocities = '2 3 4'
  []

  [particle_initializer]
    type = TestPlacedParticleInitializer
    charge = 1
    weight = 1
    velocity_initializer = 'velocity_initializer'
    start_points = '0.75 0 0'
  []

  [study]
    type = TestInitializedPICStudy
    stepper = stepper
    particle_initializers = particle_initializer
    use_custom_rayids = false
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
    ray_kernel_coverage_check = false
  []
[]

[VectorPostprocessors]
  [particle_data]
    type = TestParticleDataVectorPostprocessor
    study = study
    execute_on = TIMESTEP_END
  []
[]

[RayBCs/refect]
  type = DiffusiveReflectionBC
  boundary = 'left right'
  seed = 0
  velocity_initializer = reset_initializer
  reflection_direction = 0
[]

[Executioner]
  type = Transient
  dt = 0.5
  num_steps = 2
[]

[Outputs]
  csv = true
  execute_on = 'TIMESTEP_END'
[]

