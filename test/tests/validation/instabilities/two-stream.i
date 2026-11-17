# the total number of elements in the simulation
num_elems = 32
# the number of computational particles of each species to be put in each element
particles_per_element = 200
# permitivity of free space F / m
eps0 = 1
# electron mass in kg
m_e = 1
# number density of a single direction of electrons in 1 / m^3
n_0 = 1
# Charge of a particle in C
elemental_charge = 1
# the perturbation magnitude in the spatial distribution
eps = 1e-4
# the perturbation mode in rad / m
k = 1
# Particle velocity in m / s
v_0 = 1
# Domain length in m
L = '${fparse 2 * pi}'
# simiulation time step in s
dt = 0.05

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = ${num_elems}
    xmax = ${L}
    elem_type = EDGE2
  []
  allow_renumbering = false
[]

[Variables]
  [phi]
  []
[]

[BCs]
  [Periodic]
    [all]
      auto_direction = 'x'
    []
  []
[]

[Kernels]
  [poissons]
    type = ADMatDiffusion
    diffusivity = ${eps0}
    variable = phi
  []
  # this is the neutralizing positive background charge density
  [background]
    type = BodyForce
    variable = phi
    value = '${fparse 2 * elemental_charge * n_0}'
  []
[]

[AuxVariables]
  [Ex]
    order = CONSTANT
    family = MONOMIAL
  []
  [Ey]
  []
  [Ez]
  []
  [Emag]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [grad_potential]
    type = NegativeVariableGradientComponent
    variable = Ex
    gradient_variable = phi
    component = 0
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [compute_Emag]
    type = ParsedAux
    variable = Emag
    coupled_variables = Ex
    expression = 'abs(Ex)'
  []
[]

[UserObjects]
  [stepper]
    type = LeapFrogStepper
    field_components = 'Ex Ey Ez'
  []

  [left_velocity_initializer]
    type = ConstantVelocityInitializer
    velocities = '${fparse -v_0} 0 0'
  []

  [right_velocity_initializer]
    type = ConstantVelocityInitializer
    velocities = '${fparse v_0} 0 0'
  []

  [moving_left_particle_initializer]
    type = OneDPerturbedDensityParticleInitializer
    mass = ${m_e}
    charge = '${fparse -elemental_charge}'
    number_density = ${n_0}
    particles_per_element = '${fparse particles_per_element / 2}'
    perturbation_prefactor = ${eps}
    instability_mode = ${k}
    velocity_initializer = 'left_velocity_initializer'
    seed = 1
  []

  [moving_right_particle_initializer]
    type = OneDPerturbedDensityParticleInitializer
    mass = ${m_e}
    charge = '${fparse -elemental_charge}'
    number_density = ${n_0}
    particles_per_element = '${fparse particles_per_element / 2}'
    perturbation_prefactor = ${eps}
    instability_mode = ${k}
    velocity_initializer = 'right_velocity_initializer'
  []

  [study]
    type = TestInitializedPICStudy
    stepper = stepper
    particle_initializers = 'moving_left_particle_initializer moving_right_particle_initializer'
    always_cache_traces = true
    data_on_cache_traces = true
    particles_per_element = '${fparse 2 * particles_per_element}'
    use_custom_rayids = false
    execute_on = 'TIMESTEP_BEGIN'
  []

  [pot_charge_accumulator]
    type = ChargeDensityAccumulator
    study = study
    variable = phi
  []
[]

[Postprocessors]
  [ray_count]
    type = RayTracingStudyResult
    result = TOTAL_RAYS_STARTED
    study = study
  []

  [Emax]
    type = ElementExtremeValue
    variable = Emag
  []
[]

[VectorPostprocessors]
  [rays]
    type = TestParticleDataVectorPostprocessor
    study = study
    additional_ray_data_outputs = 'weight charge mass'
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[RayBCs]
  [periodic]
    type = PeriodicRayBC
    auto_direction = 'x'
  []
[]

[Executioner]
  type = Transient
  end_time = 25
  dt = ${dt}
  automatic_scaling = true
  compute_scaling_once = false
[]

[Outputs]
  csv = true
[]
