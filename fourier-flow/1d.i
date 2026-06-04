# reference pressure in Pa i.e. J / m^3
# P_init = 266.644
# # initial temperature in K
# T_init = 273.15
# # universal gas constant in J / (mol K)
# R_u = 8.314472
# # Avagadros number in 1 / mol
# N_A = 6.022140e23
# # Argon Molar Mass kg / kmol
# m_Ar = '${fparse 39.948 * 1e-3 / N_A}'
# # number density
# number_density = '${fparse P_init * N_A / (R_u * T_init)}'

[GlobalParams]
  seed = 0
[]

[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 1
  nx = 1
  xmax = 1
[]

[Problem]
  solve = false
[]

[AuxVariables/temperature]
  order = CONSTANT
  family = MONOMIAL
[]

# [Distributions]
#   # [initial_dist]
#   #   type = Maxwellian
#   #   mass = ${m_Ar}
#   #   temperature = ${T_init}
#   # []
# []

[UserObjects]
  [stepper]
    type = TestStationaryStepper
    # type = TestSimpleStepper
  []

  # [velocity_initializer]
  #   type = VelocitiesFromDistributionsVelocityInitializer
  #   distributions = 'initial_dist initial_dist initial_dist'
  # []

  [constant_initializer]
    type = ConstantVelocityInitializer
    velocities = '2 3 4'
  []

  [particle_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = 10
    number_density = 1
    # number_density = ${number_density}
    charge = 0
    mass = 1
    # mass = ${m_Ar}
    velocity_initializer = constant_initializer
  []
  [maxwell]
    type = MaxwellCollision
    reactants = 'A A'
    products = 'A A'
    study = study
    reference_value = 1
  []
  [collider]
    type = DSMCCollider
    study = study
    collision_objects = 'maxwell'
  []
  [study]
    type = CollisionalPICStudy
    stepper = stepper
    particle_initializers = particle_initializer
    collider = collider
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
    ray_kernel_coverage_check = false
  []
  # [temp_accum]
  #   type = PerElementAverageTemperatureAccumulator
  #   study = study
  #   species = 'B'
  #   aux_variable = temperature
  # []
[]

# [VectorPostprocessors]
#   [particle_data]
#     type = TestParticleDataVectorPostprocessor
#     additional_outputs = 'mass charge'
#     study = study
#     execute_on = TIMESTEP_END
#   []
# []

[RayBCs/refect]
  type = ReflectParticleBC
  boundary = 'left right'
  # type = DiffusiveReflectionBC
  # boundary = 'left right'
  # seed = 0
  # velocity_initializer = reset_initializer
  # reflection_direction = 0
[]

[Executioner]
  type = Transient
  dt = 2
  num_steps = 1
[]

[Outputs]
  csv = true
  # exodus = true
  execute_on = 'TIMESTEP_END'
[]

