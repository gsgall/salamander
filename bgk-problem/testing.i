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
mean_collision_time = '${fparse 1 / collision_frequency}'

dt = '${fparse 1 / 4 * mean_collision_time}'
dx = '${fparse 1 / 3 * mean_free_path}'
elems_per_dim = 1
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
  # [velocity_initializer]
  #   type = ConstantVelocityInitializer
  #   velocities = '1 1 1'
  # []
  [particle_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = ${particles_per_element}
    number_density = ${number_density}
    charge = 0
    mass = ${m}
    velocity_initializer = velocity_initializer
  []
  #  [hard_sphere]
  #    type = HardSphereCollision
  #    reactants = 'A A'
  #    products = 'A A'
  #    study = study
  #    diameter = 0.46065886596178063
  #    initial_temperature = ${T_eq}
  #  []
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
    #collision_objects = 'hard_sphere'
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
  [velocities]
    type = ParticleVelocityVectorPostprocessor
    study = study
  []
  [distribution]
    type = HistogramVectorPostprocessor
    num_bins = 250
    vpp = velocities
  []
  [particles]
    type = ParticleDataVectorPostprocessor
    study = study
  []
  # [T_x]
  #   type = SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor
  #   study = study
  #   species = 'A'
  #   component = 0
  # []
  # [T_y]
  #   type = SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor
  #   study = study
  #   species = 'A'
  #   component = 1
  # []
  # [T_z]
  #   type = SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor
  #   study = study
  #   species = 'A'
  #   component = 2
  # []
[]

# [Postprocessors]
#   [T_x]
#     type = SingleSpeciesTemperature
#     species = 'A'
#     component = 0
#   []
#   [T_y]
#     type = SingleSpeciesTemperature
#     species = 'A'
#     component = 1
#   []
#   [T_z]
#     type = SingleSpeciesTemperature
#     species = 'A'
#     component = 2
#   []
# []

[Executioner]
  type = Transient
  dt = ${dt}
  # dt = 1e-10
  # num_steps = 2
  num_steps = 1
[]

[Outputs]
  exodus = false
  [csv]
    type = CSV
    start_step = 1
    time_step_interval = 5
  []
[]
