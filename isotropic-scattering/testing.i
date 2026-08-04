[Problem]
  solve = false
[]

[GlobalParams]
  study = study
  particles_per_element = 5000
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 1
    xmax = 1
  []
  allow_renumbering = false
[]

[AuxVariables]
  [A_A_reaction]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[UserObjects]
  [stepper]
    type = TestStationaryStepper
  []
  [velocity_initializer_a]
    type = ConstantVelocityInitializer
    velocities = '-10 0 0'
  []
  [velocity_initializer_b]
    type = ConstantVelocityInitializer
    velocities = '10 0 0'
  []
  [a_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    number_density = 1
    charge = 0
    mass = 1
    velocity_initializer = velocity_initializer_a
  []
  [b_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    number_density = 1
    charge = 0
    mass = 1
    velocity_initializer = velocity_initializer_b
  []
  [maxwell_a]
    type = MaxwellHardSphereCollision
    reactants = 'A A'
    products = 'A A'
    sigma_ref = 10
  []
  [collider]
    type = DSMCCollider
    collision_objects = 'maxwell_a'
  []
  [study]
    type = PICStudy
    colliders = collider
    stepper = stepper
    particle_initializers = 'a_initializer b_initializer'
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
    ray_kernel_coverage_check = false
  []
  [A_A_accumulator]
    type = ReactionRateAuxAccumulator
    aux_variable = A_A_reaction
    collision = maxwell_a
    collider = collider
    execute_on = 'TIMESTEP_END'
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
    boundary = 'left right'
  []
[]

[Postprocessors]
  [A_A_reaction_rate]
    type = ElementAverageValue
    variable = 'A_A_reaction'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
[]

[VectorPostprocessors]
  [particles]
    type = ParticleDataVectorPostprocessor
    study = study
  []
  [distribution]
    type = HistogramVectorPostprocessor
    num_bins = 50
    vpp = particles
    column_names = 'v_x v_y v_z'
  []
[]

[Executioner]
  type = Transient
  dt = 10
  num_steps = 1
[]

[Outputs]
  exodus = false
  csv = true
[]
