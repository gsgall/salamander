[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 10
    xmin = -1
    xmax = 1
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [initializer]
    type = TestPlacedParticleInitializer
    start_points = '0.1 0 0
                    0.75 0 0'
    start_velocities = '-1 0 0
                        0.5 0 0'
    mass = 1
    weight = 1
    charge = 1
  []

  [study]
    type = PeriodicPICStudy
    stepper = stepper
    initializers = initializer
    use_custom_rayids = false
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[RayBCs]
  [periodic]
    type = KillRayBC
    boundary = 'left right'
  []
[]

[Postprocessors]
  [particle_count]
    type = RayTracingStudyResult
    result = "total_rays_started"
    study = study
  []
[]
[VectorPostprocessors]
  [particle_data]
    type = TestPeriodicParticleDataVectorPostprocessor
    study = study
    additional_ray_data_outputs = 'charge mass weight'
    execute_on = 'TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 3
[]

[Problem]
  solve = false
  kernel_coverage_check = false
[]

[Outputs]
  [particle_data]
    type = CSV
    file_base = '1d_periodic'
    execute_on = 'TIMESTEP_END'
  []
[]
