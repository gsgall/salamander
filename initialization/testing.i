[GlobalParams]
  seed = 0
  mass = 1e-20
  study = study
[]

[Problem]
  solve = false
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 10
    xmax = 10
    ymax = 10
  []
  allow_renumbering = false
[]

[AuxVariables]
  [temperature]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [null]
    type = CopyValueAux
    variable = temperature
    source = temperature
  []
[]

[Distributions]
  [v_0]
    type = Maxwellian
    temperature = 273.15
  []
[]

[UserObjects]
  [stepper]
    type = TestStationaryStepper
  []
  [velocity_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_0 v_0 v_0'
  []
  [particle_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = 10
    number_density = 1
    charge = 0
    velocity_initializer = velocity_initializer
  []

  [study]
    type = PICStudy
    stepper = stepper
    particle_initializers = particle_initializer
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
    ray_kernel_coverage_check = false
  []

  [temperature_accumulator]
    type = SingleSpeciesTemperatureAuxAccumulator
    species = 'A'
    aux_variable = temperature
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
  [element_temperatures]
    type = ElementValueSampler
    variable = 'temperature'
    sort_by = id
    execute_on = 'TIMESTEP_END'
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
[]

[Postprocessors]
  [average_temperature]
    type = VectorPostprocessorReductionValue
    vector_name = temperature
    vectorpostprocessor = element_temperatures
    value_type = average
    # this ensures that this will execute after values
    # have been accumulated by the accumulator userobject
    execution_order_group = 1
  []
[]

[Executioner]
  type = Transient
  num_steps = 1
[]

[Outputs]
  exodus = false
  [csv]
    type = CSV
    start_step = 1
  []
[]
