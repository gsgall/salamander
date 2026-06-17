[Problem]
  solve = false
[]

[GlobalParams]
  study = study
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 10
    xmax = 10
  []
  allow_renumbering = false
[]

[UserObjects]
  [stepper]
    type = TestStationaryStepper
  []
  [velocity_initializer]
    type = ConstantVelocityInitializer
    velocities = '0 0 0'
  []
  [a_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = 10
    number_density = 1
    charge = 0
    mass = 1
    velocity_initializer = velocity_initializer
  []
  [b_initializer]
    type = PerElementParticleInitializer
    species = 'B'
    particles_per_element = 10
    number_density = 1
    charge = 0
    mass = 1
    velocity_initializer = velocity_initializer
  []
  [maxwell_a]
    type = MaxwellCollision
    reactants = 'A A'
    products = 'A A'
    reference_value = 1.0
  []
  [maxwell_b]
    type = MaxwellCollision
    reactants = 'B B'
    products = 'B B'
    reference_value = 1.0
  []
  [maxwell_ab]
    type = MaxwellCollision
    reactants = 'A B'
    products = 'A B'
    reference_value = 1.0
  []
  [collider]
    type = DSMCCollider
    collision_objects = 'maxwell_a maxwell_b maxwell_ab'
  []
  [study]
    type = CollisionalPICStudy
    collider = collider
    stepper = stepper
    particle_initializers = 'a_initializer b_initializer'
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
    boundary = 'left right'
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 10
[]

[Outputs]
  exodus = false
  csv = false
[]
