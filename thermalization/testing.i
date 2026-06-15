# the number of computational particles to be put in each element
particles_per_element = 1000
# the analytic solution for the charge density
# set by cli args for each case since it is needed by all
# tests but changes based on the dimension of the problem
number_density = 3e23
m = 6.6464764e-27
k_B = 1.380649e-23
# T_alpha = 1020
T_alpha = 600
T_beta = 500
sigma_alpha = '${fparse sqrt(k_B * T_alpha / m)}'
sigma_beta = '${fparse sqrt(k_B * T_beta / m)}'
# sigma_0 = 0
sigma_0 = 1e-20
sigma_intra = '${fparse sigma_0 * 100}'

seed = 50
alpha_seed = 0
beta_seed = 10

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

[GlobalParams]
  study = study
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
    seed = ${alpha_seed}
  []

  [alpha_initializer]
    type = PerElementParticleInitializer
    mass = ${m}
    charge = 1
    species = 'alpha'
    number_density = ${number_density}
    particles_per_element = ${particles_per_element}
    velocity_initializer = alpha_vel_initializer
    seed = ${alpha_seed}
  []

  [beta_vel_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_beta v_beta v_beta'
    seed = ${beta_seed}
  []

  [beta_initializer]
    type = PerElementParticleInitializer
    mass = ${m}
    charge = 1
    species = 'beta'
    number_density = ${number_density}
    particles_per_element = ${particles_per_element}
    velocity_initializer = beta_vel_initializer
    seed = ${beta_seed}
  []
  [maxwell_alpha]
    type = MaxwellCollision
    reactants = 'alpha alpha'
    products = 'alpha alpha'
    reference_value = ${sigma_intra}
  []
  [maxwell_beta]
    type = MaxwellCollision
    reactants = 'beta beta'
    products = 'beta beta'
    reference_value = ${sigma_intra}
  []
  [maxwell_inter]
    type = MaxwellCollision
    reactants = 'alpha beta'
    products = 'alpha beta'
    reference_value = ${sigma_0}
  []
  [collider]
    type = DSMCCollider
    collision_objects = 'maxwell_alpha maxwell_beta maxwell_inter'
    seed = ${seed}
    # collision_objects = 'hard_sphere'
  []
  [study]
    type = CollisionalPICStudy
    # cross_sections = '${sigma_intra} ${sigma_0} ${sigma_intra}'
    collider = collider
    stepper = stepper
    particle_initializers = 'alpha_initializer beta_initializer'
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

# [Postprocessors]
#   [particle_count]
#     type = RayTracingStudyResult
#     result = 'total_rays_started'
#     study = study
#   []
#   [T_alpha]
#     type = SingleSpeciesTemperature
#     study = study
#     species_id = 0
#   []
#   [T_beta]
#     type = SingleSpeciesTemperature
#     study = study
#     species_id = 1
#   []
# []

[VectorPostprocessors]
  [T_alpha_elem]
    type = SingleSpeciesPerElementTemperatureVectorPostprocessor
    study = study
    species = 'alpha'
    # num_elems = 16
  []
  [T_beta_elem]
    type = SingleSpeciesPerElementTemperatureVectorPostprocessor
    study = study
    species = 'beta'
    # num_elems = 16
  []
[]

[Executioner]
  type = Transient
  dt = 1e-6
  num_steps = 3000
  # num_steps = 1
[]

[Outputs]
  exodus = false
  # csv = true
  time_step_interval = 5
  [csv]
    type = CSV
    execute_on = 'FINAL'
  []
[]
