T_0 = 273.15
m = 1e-20


[Problem]
  solve = false
[]

[GlobalParams]
  seed = 9812435
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

[Distributions]
  [v_0]
    type = Maxwellian
    mass = ${m}
    temperature = ${T_0}
  []
[]

[UserObjects]
  [stepper]
    #type = TestSimpleStepper
    type = TestStationaryStepper
  []
  [velocity_initializer]
    type = VelocitiesFromDistributionsVelocityInitializer
    distributions = 'v_0 v_0 v_0'
  []
  [particle_initializer]
    type = PerElementParticleInitializer
    species = 'A'
    particles_per_element = 1000
    number_density = 1
    charge = 0
    mass = ${m}
    velocity_initializer = velocity_initializer
  []
  [study]
    type = CollisionlessPICStudy
    stepper = stepper
    particle_initializers = particle_initializer
    #collider = collider
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
  [T_x]
    type = SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor
    study = study
    species = 'A'
    component = 0
  []
  [T_y]
    type = SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor
    study = study
    species = 'A'
    component = 1
  []
  [T_z]
    type = SingleSpeciesPerElementPerComponentTemperatureVectorPostprocessor
    study = study
    species = 'A'
    component = 2
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 1
[]

[Outputs]
  exodus = false
  [csv]
    type = CSV
    start_step = 1
  []
[]
