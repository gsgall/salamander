!include initializer_base.i

[Mesh]
  [gmg]
    dim = 2
    nx = 10
    ny = 10
    xmax = 1
    ymax=1
  []
[]

[BCs]
  [potential_solution]
    type = FunctionDirichletBC
    variable = phi
    function = potential
    boundary = 'left right top bottom'
  []
[]

[Functions]
  [potential]
    type = ParsedFunction
    expression = 'x * (1 - x) + y * (1 - y)'
  []
[]

