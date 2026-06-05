# reference pressure in Pa i.e. J / m^3
P_init = 266.644
# initial temperature in K
T_init = 273.15
# temperature difference
delta_T = 100
# universal gas constant in J / (mol K)
R_u = 8.314472
# Avagadros number in 1 / mol
N_A = 6.022140e23
# Argon Molar Mass kg / kmol
m_Ar = '${fparse 39.948 * 1e-3 / N_A}'
# number density
number_density = '${fparse P_init * N_A / (R_u * T_init)}'
mean_free_path = 0.0237e-3
L = '${fparse 42 * mean_free_path}'
d_ref = 3.658e-10
sigma = '${fparse pi * d_ref^2 / 4.0}'

[GlobalParams]
  seed = 0
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 400
    xmax = ${L}
  []
  allow_renumbering = false
[]

[Problem]
  solve = false
[]

[AuxVariables]
  [temperature]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Distributions]
  [initial_dist]
    type = Maxwellian
    mass = ${m_Ar}
    temperature = ${T_init}
  []
  [left_dist]
    type = Maxwellian
    mass = ${m_Ar}
    temperature = '${fparse T_init - delta_T / 2.0}'
  []
  [right_dist]
    type = Maxwellian
    mass = ${m_Ar}
    temperature = '${fparse T_init + delta_T / 2.0}'
  []
[]

[UserObjects]
  [stepper]
    # type = TestStationaryStepper
    type = TestSimpleStepper
  []

  [velocity_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'initial_dist initial_dist initial_dist'
  []
  [left_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'left_dist left_dist left_dist'
  []
  [right_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'right_dist right_dist right_dist'
  []

  [particle_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = 30
    # number_density = 1
    number_density = ${number_density}
    charge = 0
    # mass = 1
    mass = ${m_Ar}
    # velocity_initializer = constant_initializer
    velocity_initializer = velocity_initializer
  []
  [maxwell]
    type = MaxwellCollision
    reactants = 'A A'
    products = 'A A'
    study = study
    reference_value = ${sigma}
  []
  [collider]
    type = DSMCCollider
    study = study
    collision_objects = 'maxwell'
  []
  [study]
    type = CollisionalPICStudy
    # type = CollisionlessPICStudy
    stepper = stepper
    particle_initializers = particle_initializer
    collider = collider
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
    ray_kernel_coverage_check = false
  []
  [temp_accum]
    type = PerElementAverageTemperatureAccumulator
    study = study
    species = 'A'
    aux_variable = temperature
  []
[]

[VectorPostprocessors]
  # [particle_data]
  #   type = TestParticleDataVectorPostprocessor
  #   additional_outputs = 'mass charge'
  #   study = study
  #   execute_on = TIMESTEP_END
  # []
  [T_A]
    type = SingleSpeciesPerElementTemperatureVectorPostprocessor
    study = study
    species = 'A'
    num_elems = 400
  []
[]

[RayBCs]
  [left]
    type = DiffusiveReflectionBC
    boundary = 'left'
    velocity_initializer = left_initializer
    reflection_direction = 0
  []
  [right]
    type = DiffusiveReflectionBC
    boundary = 'right'
    velocity_initializer = right_initializer
    reflection_direction = 0
  []
[]

[Executioner]
  type = Transient
  dt = 3.5e-6
  num_steps = 10000
[]

[Outputs]
  exodus = true
  [csv]
    type = CSV
    execute_on = 'FINAL'
  []
  execute_on = 'TIMESTEP_END'
[]

