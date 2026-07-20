[Problem]
  solve = false
[]

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 1
    xmax = 1
  []
  allow_renumbering = false
[]

[AuxVariables]
  [u]
    order = CONSTANT
    family = MONOMIAL
  []
  [time_averaged_u]
    order = CONSTANT
    family = MONOMIAL
  []
  [exact_u]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [time]
    type = FunctionAux
    variable = u
    function = time_function
  []
  [time_average]
    type = VariableTimeAverageAux
    variable = time_averaged_u
    variable_to_integrate = u
    time_step_start = 5
  []
  [exact_u]
    type = FunctionAux
    variable = exact_u
    function = analytical_int
  []
[]

[Functions]
  [time_function]
    type = ParsedFunction
    expression = 't'
  []
  [analytical_int]
    type = ParsedFunction
    expression = '((t - 1) / 2) * (1 + t) / (t)'
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 10
[]

[Outputs]
  exodus = true
[]

#[Debug]
#  show_execution_order = ALWAYS
#[]
