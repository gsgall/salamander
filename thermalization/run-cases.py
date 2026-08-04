import mooseutils
import os
from random import seed, randint


input_file = "./testing.i"
mpi = 10
executeable = "../salamander-opt"
supress_output = False

seed(0)

runs = 33

for i in range(10):
    randint(0, int(1e8))

file_path = "./data"
if not os.path.exists(file_path):
    os.makedirs(file_path)
for i in range(runs):
    print(f"Run: {i}")
    file_base = f"{file_path:s}/run_{i:d}"
    a = ["-i", input_file,
         "--allow-unused",
         "--allow-test-objects",
         f"GlobalParams/seed={randint(0, int(1e8))}",
         f"Outputs/file_base={file_base}",
         "Outputs/console=false"
         ]

    mooseutils.run_executable(
        executeable, *a, mpi=mpi, suppress_output=supress_output)
