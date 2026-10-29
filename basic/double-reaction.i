[Problem]
  solve = false
[]

[GlobalParams]
  study = study
  particles_per_element = 10
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
  [A_A_reaction_1]
    order = CONSTANT
    family = MONOMIAL
  []
  [A_A_reaction_2]
    order = CONSTANT
    family = MONOMIAL
  []
  [B_B_reaction_1]
    order = CONSTANT
    family = MONOMIAL
  []
  [B_B_reaction_2]
    order = CONSTANT
    family = MONOMIAL
  []
  [A_B_reaction_1]
    order = CONSTANT
    family = MONOMIAL
  []
  [A_B_reaction_2]
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
    number_density = 3
    charge = 0
    mass = 1
    velocity_initializer = velocity_initializer
  []
  [b_initializer]
    type = PerElementParticleInitializer
    species = 'B'
    number_density = 3
    charge = 0
    mass = 1
    velocity_initializer = velocity_initializer
  []
  [maxwell_a_1]
    type = MaxwellHardSphereCollision
    reactants = 'A A'
    products = 'A A'
    sigma_ref = 1.0
  []
  [maxwell_a_2]
    type = MaxwellHardSphereCollision
    reactants = 'A A'
    products = 'A A'
    sigma_ref = 2.0
  []
  [maxwell_b_1]
    type = MaxwellHardSphereCollision
    reactants = 'B B'
    products = 'B B'
    sigma_ref = 3.0
  []
  [maxwell_b_2]
    type = MaxwellHardSphereCollision
    reactants = 'B B'
    products = 'B B'
    sigma_ref = 4.0
  []
  [maxwell_ab_1]
    type = MaxwellHardSphereCollision
    reactants = 'A B'
    products = 'A B'
    sigma_ref = 5.0
  []
  [maxwell_ab_2]
    type = MaxwellHardSphereCollision
    reactants = 'A B'
    products = 'A B'
    sigma_ref = 6.0
  []
  [collider]
    type = DSMCCollider
    collision_objects = 'maxwell_a_1 maxwell_a_2 maxwell_b_1 maxwell_b_2 maxwell_ab_1 maxwell_ab_2'
    #collision_objects = 'maxwell_a_1 maxwell_b_1 maxwell_ab_1'
    #collision_objects = 'maxwell_a_1 maxwell_a_2 maxwell_b_1 maxwell_ab_1'
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
  [A_A_accumulator_1]
    type = ReactionRateAuxAccumulator
    aux_variable = A_A_reaction_1
    collision = maxwell_a_1
    collider = collider
    execute_on = 'TIMESTEP_END'
  []
  [B_B_accumulator_1]
    type = ReactionRateAuxAccumulator
    aux_variable = B_B_reaction_1
    collision = maxwell_b_1
    collider = collider
    execute_on = 'TIMESTEP_END'
  []
  [A_B_accumulator_1]
    type = ReactionRateAuxAccumulator
    aux_variable = A_B_reaction_1
    collision = maxwell_ab_1
    collider = collider
    execute_on = 'TIMESTEP_END'
  []
  [A_A_accumulator_2]
    type = ReactionRateAuxAccumulator
    aux_variable = A_A_reaction_2
    collision = maxwell_a_2
    collider = collider
    execute_on = 'TIMESTEP_END'
  []
  [B_B_accumulator_2]
    type = ReactionRateAuxAccumulator
    aux_variable = B_B_reaction_2
    collision = maxwell_b_2
    collider = collider
    execute_on = 'TIMESTEP_END'
  []
  [A_B_accumulator_2]
    type = ReactionRateAuxAccumulator
    aux_variable = A_B_reaction_2
    collision = maxwell_ab_2
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
  [A_A_reaction_rate_1]
    type = ElementAverageValue
    variable = 'A_A_reaction_1'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [A_A_reaction_rate_2]
    type = ElementAverageValue
    variable = 'A_A_reaction_2'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [B_B_reaction_rate_1]
    type = ElementAverageValue
    variable = 'B_B_reaction_1'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [B_B_reaction_rate_2]
    type = ElementAverageValue
    variable = 'B_B_reaction_2'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [A_B_reaction_rate_1]
    type = ElementAverageValue
    variable = 'A_B_reaction_1'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [A_B_reaction_rate_2]
    type = ElementAverageValue
    variable = 'A_B_reaction_2'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 100
[]

[Outputs]
  exodus = true
  csv = true
[]
