!include initializer_base.i

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 10
    xmax = 1
    ymax=1
  []
  allow_renumbering = false
[]

[BCs]
  [potential_solution]
    type = FunctionDirichletBC
    variable = phi
    function = potential
    boundary = 'left right top bottom'
  []
[]


