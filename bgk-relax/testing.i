particles_per_element = 1000
T_0 = 273.15
T_12 = 373.15
T_eq = '${fparse (T_0 + 2.0 * T_12) / 3.0 }'
m = 1e-20
sigma_0 = 1
number_density = 1
k_B = 1.380649e-23

mean_vel = '${fparse sqrt(8 * k_B * T_eq / (pi * m))}'
#mean_rel_vel = '${fparse sqrt(2) * mean_vel}'
sigma_g_bar = ${sigma_0}
collision_frequency = '${fparse number_density * sigma_g_bar}'
mean_free_path = '${fparse mean_vel / collision_frequency}'
#mean_collision_time = '${fparse 1 / collision_frequency}'

#dt = '${fparse 1 / 4 * mean_collision_time}'
dx = '${fparse 1 / 3 * mean_free_path}'
elems_per_dim = 10
L = '${fparse elems_per_dim * dx}'

[Problem]
  solve = false
[]

[GlobalParams]
  seed = 9812435
  study = study
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = ${elems_per_dim}
    ny = ${elems_per_dim}
    xmax = ${L}
    ymax = ${L}
  []
  allow_renumbering = false
[]

[AuxVariables]
  [T_x]
    order = CONSTANT
    family = MONOMIAL
  []
  [T_y]
    order = CONSTANT
    family = MONOMIAL
  []
  [T_z]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Distributions]
  [v_0]
    type = Maxwellian
    mass = ${m}
    temperature = ${T_0}
  []
  [v_12]
    type = Maxwellian
    mass = ${m}
    temperature = ${T_12}
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []
  [velocity_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_0 v_12 v_12'
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
  [hard_sphere]
    type = HardSphereCollision
    reactants = 'A A'
    products = 'A A'
    study = study
    diameter = 0.46065886596178063
  []
  [maxwell]
    type = MaxwellHardSphereCollision
    reactants = 'A A'
    products = 'A A'
    study = study
    sigma_ref = ${sigma_0}
  []
  [collider]
    type = DSMCCollider
    study = study
    #collision_objects = 'maxwell'
    collision_objects = 'hard_sphere'
  []
  [study]
    type = PICStudy
    colliders = collider
    stepper = stepper
    particle_initializers = particle_initializer
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
    ray_kernel_coverage_check = false
  []
  [T_x_accum]
    type = SingleSpeciesSingleComponentTemperature
    species = 'A'
    aux_variable = T_x
    component = 0
  []
  [T_y_accum]
    type = SingleSpeciesSingleComponentTemperature
    species = 'A'
    aux_variable = T_y
    component = 1
  []
  [T_z_accum]
    type = SingleSpeciesSingleComponentTemperature
    species = 'A'
    aux_variable = T_z
    component = 2
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
    boundary = 'left right top bottom'
  []
[]

[VectorPostprocessors]
  [particles]
    type = ParticleDataVectorPostprocessor
    study = study
  []
  [distribution]
    type = CustomHistogramVectorPostprocessor
    num_bins = 75
    vpp = particles
    column_names = 'v_x v_y v_z'
    lower_bound = -3
    upper_bound = 3
  []
[]

[Postprocessors]
  [T_x]
    type = ElementAverageValue
    variable = 'T_x'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [T_y]
    type = ElementAverageValue
    variable = 'T_y'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [T_z]
    type = ElementAverageValue
    variable = 'T_z'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
[]

[Executioner]
  type = Transient
  dt = 1e-2
  num_steps = 50
[]

[Outputs]
  exodus = false
  [csv]
    type = CSV
    start_step = 1
    time_step_interval = 5
    hide = particles
  []
[]
