particles_per_element = 1000
m = 1e-20

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
    dim = 1
    nx = 11
    xmax = 100
  []
  allow_renumbering = false
[]

[AuxVariables]
  [heat_flux]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []
  [velocity_initializer]
    type = ConstantVelocityInitializer
    velocities = '100 0 0'
  []
  [velocity_initializer2]
    type = ConstantVelocityInitializer
    velocities = '-100 0 0'
  []
  [particle_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = ${particles_per_element}
    number_density = 15
    charge = 0
    mass = ${m}
    velocity_initializer = velocity_initializer
  []
  [particle_initializer2]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = ${particles_per_element}
    number_density = 16
    charge = 0
    mass = ${m}
    velocity_initializer = velocity_initializer2
  []
  [study]
    type = PICStudy
    stepper = stepper
    particle_initializers = 'particle_initializer particle_initializer2'
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
  [left_change]
    type = ChangeRayVelocity
    boundary = 'left'
    velocity = '10 0 0'
  []
  [right_change]
    type = ChangeRayVelocity
    boundary = 'right'
    velocity = '-10 0 0'
  []
  [heat_flux]
    type = BoundaryHeatFluxAccumulatorRayBC
    boundary = 'left right'
    aux_variable = 'heat_flux'
    time_step_start = 0
  []
[]

[Postprocessors]
  [left_heat_flux]
    type = PointValue
    variable = 'heat_flux'
    point = '0 0 0'
    execution_order_group = 1
    execute_on = 'TIMESTEP_END'
  []
  [right_heat_flux]
    type = PointValue
    variable = 'heat_flux'
    point = '100 0 0'
    execution_order_group = 1
    execute_on = 'TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 1
[]

[Outputs]
  exodus = true
[]
