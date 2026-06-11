particles_per_element = 100
T_0 = 273.15
delta_T = 100
T_left = '${fparse  T_0 - delta_T / 2.0}'
T_right = '${fparse  T_0 + delta_T / 2.0}'
m = 1e-20
sigma_0 = 1
number_density = 1
k_B = 1.380649e-23

mean_vel = '${fparse sqrt(8 * k_B * T_0 / (pi * m))}'
#mean_rel_vel = '${fparse sqrt(2) * mean_vel}'
sigma_g_bar = ${sigma_0}
collision_frequency = '${fparse number_density * sigma_g_bar}'
mean_free_path = '${fparse mean_vel / collision_frequency}'
mean_collision_time = '${fparse 1 / collision_frequency}'

dt = '${fparse 1 / 4 * mean_collision_time}'
dx = '${fparse 1 / 3 * mean_free_path}'
L = '${fparse 42 * mean_free_path}'

[Problem]
  solve = false
[]

[GlobalParams]
  seed = 9812435
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 126
    xmax = ${L}
  []
  allow_renumbering = false
[]

[AuxVariables]
  [temperature]
    order = CONSTANT
    family = MONOMIAL
  []
  [time_averaged_temperature]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Distributions]
  [v_ic]
    type = Maxwellian
    mass = ${m}
    temperature = ${T_0}
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
    #type = TestStationaryStepper
  []
  [velocity_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_ic v_ic v_ic'
  []
  [particle_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = ${particles_per_element}
    number_density = ${number_density}
    charge = 0
    mass = ${m}
    velocity_initializer = velocity_initializer
  []
  [maxwell]
    type = MaxwellCollision
    reactants = 'A A'
    products = 'A A'
    study = study
    reference_value = ${sigma_0}
  []
  [collider]
    type = DSMCCollider
    study = study
    collision_objects = 'maxwell'
  []
  [study]
    type = CollisionalPICStudy
    #type = CollisionlessPICStudy
    collider = collider
    stepper = stepper
    particle_initializers = particle_initializer
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
  [time_avg_temp_accum]
    type = PerElementTimeAveragedTemperatureAccumulator
    study = study
    species = 'A'
    start_averaging_step = 2000
    aux_variable = time_averaged_temperature
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[RayBCs]
  [left]
    type = DiffusiveReflectionBC
    boundary = 'left'
    temperature = ${T_left}
  []
  [right]
    type = DiffusiveReflectionBC
    boundary = 'right'
    temperature = ${T_right}
  []
  #  [walls]
  #    type = ReflectParticleBC
  #    boundary = 'left right'
  #  []
[]

[Executioner]
  type = Transient
  dt = ${dt}
  # dt = 1e-10
  # num_steps = 2
  num_steps = 10000
[]

[Outputs]
  exodus = true
[]
