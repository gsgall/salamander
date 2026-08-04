# the number of computational particles to be put in each element
particles_per_element = 1000
# the analytic solution for the charge density
# set by cli args for each case since it is needed by all
# tests but changes based on the dimension of the problem
number_density = 3e23
m = 6.6464764e-27
T_alpha = 600
T_beta = 500
sigma_0 = 1e-20
sigma_intra = '${fparse sigma_0 * 100}'


[Problem]
  solve = false
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 2
    ny = 2
    xmin = -1e-2
    xmax = 1e-2
    ymin = -1e-2
    ymax = 1e-2
  []
  allow_renumbering = false
[]

[AuxVariables]
  [T_alpha]
    order = CONSTANT
    family = MONOMIAL
  []
  [T_beta]
    order = CONSTANT
    family = MONOMIAL
  []
  [T_alpha_pec]
    order = CONSTANT
    family = MONOMIAL
  []
  [T_beta_pec]
    order = CONSTANT
    family = MONOMIAL
  []
  [alpha_rate]
    order = CONSTANT
    family = MONOMIAL
  []
  [beta_rate]
    order = CONSTANT
    family = MONOMIAL
  []
  [alpha_beta_rate]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[GlobalParams]
  study = study
  seed = 0
[]

[Distributions]
  [v_alpha]
    type = Maxwellian
    mass = ${m}
    temperature = ${T_alpha}
  []
  [v_beta]
    type = Maxwellian
    mass = ${m}
    temperature = ${T_beta}
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [alpha_vel_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_alpha v_alpha v_alpha'
  []

  [alpha_initializer]
    type = PerElementParticleInitializer
    mass = ${m}
    charge = 1
    species = 'alpha'
    number_density = ${number_density}
    particles_per_element = ${particles_per_element}
    velocity_initializer = alpha_vel_initializer
  []

  [beta_vel_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_beta v_beta v_beta'
  []

  [beta_initializer]
    type = PerElementParticleInitializer
    mass = ${m}
    charge = 1
    species = 'beta'
    number_density = ${number_density}
    particles_per_element = ${particles_per_element}
    velocity_initializer = beta_vel_initializer
  []
  [maxwell_alpha]
    type = MaxwellHardSphereCollision
    reactants = 'alpha alpha'
    products = 'alpha alpha'
    sigma_ref = ${sigma_intra}
  []
  [maxwell_beta]
    type = MaxwellHardSphereCollision
    reactants = 'beta beta'
    products = 'beta beta'
    sigma_ref = ${sigma_intra}
  []
  [maxwell_inter]
    type = MaxwellHardSphereCollision
    reactants = 'alpha beta'
    products = 'alpha beta'
    sigma_ref = ${sigma_0}
  []
  [collider]
    type = DSMCCollider
    collision_objects = 'maxwell_alpha maxwell_beta maxwell_inter'
  []
  [study]
    type = PICStudy
    colliders = collider
    stepper = stepper
    particle_initializers = 'alpha_initializer beta_initializer'
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = TIMESTEP_BEGIN
    tolerate_failure = false
    ray_kernel_coverage_check = false
  []
  [T_alpha_accum]
    type = SingleSpeciesTemperatureAuxAccumulator
    aux_variable = 'T_alpha'
    species = 'alpha'
  []
  [T_beta_accum]
    type = SingleSpeciesTemperatureAuxAccumulator
    aux_variable = 'T_beta'
    species = 'beta'
  []
  [T_alpha_pec_accum]
    type = SingleSpeciesPeculiarTemperatureAuxAccumulator
    aux_variable = 'T_alpha_pec'
    species = 'alpha'
  []
  [T_beta_pec_accum]
    type = SingleSpeciesPeculiarTemperatureAuxAccumulator
    aux_variable = 'T_beta_pec'
    species = 'beta'
  []
  [A_A_accumulator]
    type = ReactionRateAuxAccumulator
    aux_variable = alpha_rate
    collision = maxwell_alpha
    collider = collider
    execute_on = 'TIMESTEP_END'
  []
  [B_B_accumulator]
    type = ReactionRateAuxAccumulator
    aux_variable = beta_rate
    collision = maxwell_beta
    collider = collider
    execute_on = 'TIMESTEP_END'
  []
  [A_B_accumulator]
    type = ReactionRateAuxAccumulator
    aux_variable = alpha_beta_rate
    collision = maxwell_inter
    collider = collider
    execute_on = 'TIMESTEP_END'
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
  []
  [T_alpha]
    type = ElementAverageValue
    variable = 'T_alpha'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [T_beta]
    type = ElementAverageValue
    variable = 'T_beta'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [T_alpha_pec]
    type = ElementAverageValue
    variable = 'T_alpha_pec'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [T_beta_pec]
    type = ElementAverageValue
    variable = 'T_beta_pec'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [alpha_rate]
    type = ElementAverageValue
    variable = 'alpha_rate'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [beta_rate]
    type = ElementAverageValue
    variable = 'beta_rate'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
  [alpha_beta_rate]
    type = ElementAverageValue
    variable = 'alpha_beta_rate'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
[]

[Executioner]
  type = Transient
  dt = 1e-5
  num_steps = 2000
[]

[Outputs]
  exodus = false
  csv = true
[]
