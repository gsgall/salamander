# reference pressure in Pa i.e. J / m^3
P_init = 264.96
# initial temperature in K
T_init = 273.15
# universal gas constant in J / (mol K)
R_u = 8.314472
# Avagadros number in 1 / mol
N_A = 6.022140e23
#kArgon Molar Mass kg / kmol
m = 6.63e-26
# number density
number_density = '${fparse P_init * N_A / (R_u * T_init)}'

# Temperature difference between walls
delta_T = 100
# total gap distance in m
L = 1e-3
# L = 0.5e-3

# parameters for hard sphere interactions
omega = 0.5
# corresponds to isotropic scattering
alpha = 1
mu_inf_mu_1 = 1.016034
mu_1_mu_inf = '${fparse 1 / mu_inf_mu_1}'
mu_ref = 2.117e-5

k_B = 1.380658e-23
numerator = '${fparse 5 * (alpha + 1) * (alpha + 2) * sqrt(m * k_B * T_init / pi)}'
denominator = '${fparse 4 * alpha * (5 - 2 * omega) * (7 - 2 * omega) * mu_ref * (mu_1_mu_inf)}'
d_ref = '${fparse sqrt(numerator / denominator)}'

dt = 5e-8

# number_density = '${fparse P_init / (k_B * T_init)}'

[Problem]
  solve = false
[]

[GlobalParams]
  seed = 0
  study = study
  time_step_start = 3000
  #time_step_start =
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 200
    xmax = ${L}
  []
  allow_renumbering = false
[]

[AuxVariables]
  [true_temp]
    order = CONSTANT
    family = MONOMIAL
  []
  [temperature]
    order = CONSTANT
    family = MONOMIAL
  []
  [time_averaged_temperature]
    order = CONSTANT
    family = MONOMIAL
  []
  [time_averaged_heat_flux]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [true_temp]
    type = FunctionAux
    variable = true_temp
    function = temperature_profile
  []
[]

[Functions]
  [temperature_profile]
    type = ParsedFunction
    symbol_names = 'T_init delta_T L'
    symbol_values = '${T_init} ${delta_T} ${L}'
    expression = 'T_init - (delta_T / 2.0) + (delta_T / L) * x'
  []
[]

[Distributions]
  [v_0]
    type = Maxwellian
    mass = ${m}
    temperature = ${T_init}
  []
[]

[UserObjects]
  [stepper]
    #type = TestStationaryStepper
    type = TestSimpleStepper
  []
  [velocity_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_0 v_0 v_0'
  []
  [particle_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = 30
    number_density = ${number_density}
    charge = 0
    mass = ${m}
    velocity_initializer = velocity_initializer
  []
  [hard_sphere]
    type = HardSphereCollision
    reactants = 'A A'
    products = 'A A'
    diameter = ${d_ref}
  []
  [collider]
    type = DSMCCollider
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
    tolerate_failure = true
  []
  [temperature_accum]
    type = SingleSpeciesPeculiarTemperatureAuxAccumulator
    species = 'A'
    aux_variable = temperature
  []
  [time_averaged_temperature_acccum]
    type = TimeAveragedSingleSpeciesPeculiarTemperatureAuxAccumulator
    species = 'A'
    aux_variable = time_averaged_temperature
  []
[]

[VectorPostprocessors]
  [average_temperature]
    type = ElementValueSampler
    variable = 'time_averaged_temperature'
    sort_by = 'id'
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
[]

[Postprocessors]
  [left_heat_flux]
    type = PointValue
    variable = 'time_averaged_heat_flux'
    point = '0 0 0'
    execution_order_group = 1
    execute_on = 'TIMESTEP_END'
  []
  [right_heat_flux]
    type = PointValue
    variable = 'time_averaged_heat_flux'
    point = '${L} 0 0'
    execution_order_group = 1
    execute_on = 'TIMESTEP_END'
  []
[]

[RayBCs]
  [left]
    type = DiffusiveReflectParticleBC
    boundary = 'left'
    temperature = '${fparse T_init - delta_T / 2.0}'
  []
  [right]
    type = DiffusiveReflectParticleBC
    boundary = 'right'
    temperature = '${fparse T_init + delta_T / 2.0}'
  []
  [heat_flux]
    type = BoundaryHeatFluxAccumulatorRayBC
    boundary = 'left right'
    aux_variable = time_averaged_heat_flux
  []
[]

[Executioner]
  type = Transient
  dt = ${dt}
  num_steps = 6000
  #num_steps = 1
[]

[Outputs]
  exodus = true
  [csv]
    type = CSV
    execute_on = 'FINAL'
  []
[]

#[Debug]
#  show_execution_order = ALWAYS
#[]
