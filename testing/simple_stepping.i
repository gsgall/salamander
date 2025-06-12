
[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 5
    ny = 5
    xmax = 10
    ymax = 10
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [initializer]
    type = TestPlacedParticleInitializer
    start_points = '0.1 0.1 0'
    start_velocities = '1 3 0'
  []

  [study]
    type = TestInitializedPICStudy
    stepper = stepper
    initializers = initializer
    use_custom_rayids = false
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
  []
[]

[RayBCs]
  [reflect]
    type = ReflectRayBC
    boundary = 'top bottom right left'
    study = study
  []
[]
[Postprocessors]
  [elem_crossing]
    type = MeanElementsCrossedPostprocessor
    study = study
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[Executioner]
  type = Transient
  num_steps = 1
  dt = 4.5
[]

[Problem]
  solve = false
[]

[Outputs]
  [rays]
    type = RayTracingExodus
    study = study
    output_data_names = 'v_x v_y v_z weight'
    execute_on = TIMESTEP_BEGIN
  []
[]
