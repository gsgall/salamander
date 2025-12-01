# the number of computational particles to be put in each element
particles_per_element = 5
# the analytic solution for the charge density
# set by cli args for each case since it is needed by all
# tests but changes based on the dimension of the problem
number_density = 1e25
m = 6.6464764e-27
k_B = 1.380649e-23
# T_alpha = 1020
T_alpha = 1444
T_beta = 145
sigma_alpha = '${fparse sqrt(k_B * T_alpha / m)}'
sigma_beta = '${fparse sqrt(k_B * T_beta / m)}'
#sigma_0 = 0
sigma_0 = 1e-21
sigma_intra = '${fparse sigma_0 * 100}'

seed = 1234987
vel_seed_alpha = 0
vel_seed_beta = 0
[Problem]
  solve = false
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 4
    ny = 4
    xmin = -1e-2
    xmax = 1e-2
    ymin = -1e-2
    ymax = 1e-2
  []
  allow_renumbering = false
[]

[Distributions]
  [v_alpha]
    type = Normal
    mean = 0
    standard_deviation = '${sigma_alpha}'
  []
  [v_beta]
    type = Normal
    mean = 0
    standard_deviation = '${sigma_beta}'
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
    # type = TestStationaryStepper
  []

  [alpha_vel_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_alpha v_alpha v_alpha'
    seed = ${vel_seed_alpha}
  []

  [alpha_initializer]
    type = PerElementParticleInitializer
    mass = ${m}
    charge = 1
    species = 'alpha'
    number_density = ${number_density}
    particles_per_element = ${particles_per_element}
    velocity_initializer = alpha_vel_initializer
    seed = ${seed}
  []

  [beta_vel_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_beta v_beta v_beta'
    seed = ${vel_seed_beta}
  []

  [beta_initializer]
    type = PerElementParticleInitializer
    mass = ${m}
    charge = 1
    species = 'beta'
    number_density = ${number_density}
    particles_per_element = ${particles_per_element}
    velocity_initializer = beta_vel_initializer
    seed = ${seed}
  []

  [study]
    type = CollisionalPICStudy
    cross_sections = '${sigma_intra} ${sigma_0} ${sigma_intra}'
    stepper = stepper
    initializers = 'alpha_initializer beta_initializer'
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = TIMESTEP_BEGIN
    tolerate_failure = false
    ray_kernel_coverage_check = false
  []
[]

[RayBCs]
  [walls]
    type = ReflectParticleBC
    boundary = 'left right top bottom'
  []
[]

[Postprocessors]

  [particle_count]
    type = RayTracingStudyResult
    result = 'total_rays_started'
    study = study
  []

  [T_alpha]
    type = SingleSpeciesTemperature
    study = study
    species_id = 0
  []
  [T_beta]
    type = SingleSpeciesTemperature
    study = study
    species_id = 1
  []
[]
#[VectorPostprocessors]
#  [alpha_speeds]
#    type = SingleSpeciesSpeedVectorPostprocessor
#    study = study
#    species_id = 0
#  []
#  [beta_speeds]
#    type = SingleSpeciesSpeedVectorPostprocessor
#    study = study
#    species_id = 1
#  []
#  # [distribution]
#  #   type = HistogramVectorPostprocessor
#  #   num_bins = 250
#  #   vpp = velocities
#  # []
#[]

[Executioner]
  type = Transient
  dt = 1e-7
  # num_steps = 100
  num_steps = 100
[]

[Outputs]
  exodus = false
  csv = true
  # [csv]
  #   type = CSV
  #   show = 'distribution particle_count'
  # []
  #interval = 10
[]
