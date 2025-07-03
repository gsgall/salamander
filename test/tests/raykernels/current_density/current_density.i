[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 2
    xmin = -1
    xmax = 1
  []
[]

[Problem]
  extra_tag_vectors = zero_dump
  not_zeroed_tag_vectors = 'J_residual'
[]

[Variables]
  [unused]
  []
[]

[Kernels]
  [null]
    type = NullKernel
    variable = unused
  []
[]

[AuxVariables]
  [J_residual]
  []
[]

[AuxKernels]
  [J_residual]
    type = TagVectorAux
    variable = J_residual
    vector_tag = J_residual
    v = unused
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [initializer]
    type = TestPlacedParticleInitializer
    start_points = '-1 0 0'
    start_velocities = '2 0 0'
    charge = 1.5
    weight = 2
  []

  [study]
    type = TestInitializedPICStudy
    stepper = stepper
    initializer = initializer
    use_custom_rayids = false
    always_cache_traces = true
    data_on_cache_traces = true
    current_density_residual_vector_tag = zero_dump
    execute_on = 'TIMESTEP_BEGIN PRE_KERNELS'
  []
[]

[RayKernels]
  [current]
    type = ADCurrentDensityRayKernel
    variable = unused
    vector_tags = J_residual
    component = 0
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 1
[]

[Outputs]
  exodus = true
[]
