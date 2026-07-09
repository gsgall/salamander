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
  [B_B_reaction]
    order = CONSTANT
    family = MONOMIAL
  []
  [A_B_reaction]
    order = CONSTANT
    family = MONOMIAL
  []
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
    type = MaxwellHardSphereCollision
    reactants = 'A A'
    products = 'A A'
    sigma_ref = 1.0
  []
  [maxwell_b]
    type = MaxwellHardSphereCollision
    reactants = 'B B'
    products = 'B B'
    sigma_ref = 1.0
  []
  [maxwell_ab]
    type = MaxwellHardSphereCollision
    reactants = 'A B'
    products = 'A B'
    sigma_ref = 1.0
  []
  [collider]
    type = DSMCCollider
    collision_objects = 'maxwell_a maxwell_b maxwell_ab'
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
  [B_B_accumulator]
    type = ReactionRateAuxAccumulator
    aux_variable = B_B_reaction
    collision = maxwell_b
    collider = collider
    execute_on = 'TIMESTEP_END'
  []
  [A_B_accumulator]
    type = ReactionRateAuxAccumulator
    aux_variable = A_B_reaction
    collision = maxwell_ab
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
  [B_B_reaction_rate]
    type = ElementAverageValue
    variable = 'B_B_reaction'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [A_B_reaction_rate]
    type = ElementAverageValue
    variable = 'A_B_reaction'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 10
[]

[Outputs]
  exodus = true
  csv = true
[]
