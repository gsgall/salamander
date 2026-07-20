
import mooseutils
import os
from random import seed, randint


input_file = "./real.i"
mpi = 1
executeable = "../salamander-opt"
supress_output = False

seed(0)

runs = 20
particles_per_element = [30]

for i in range(10):
    randint(0, int(1e8))

for ppe in particles_per_element:
    file_path = f"./{ppe:d}_ppe"
    if not os.path.exists(file_path):
        os.makedirs(file_path)
    for i in range(runs):
        print(f"Run: {i}")
        file_base = f"{file_path:s}/run_{i:d}"
        a = ["-i", input_file, "--allow-unused",
             "--allow-test-objects",
             f"GlobalParams/seed={randint(0, int(1e8))}",
             f"UserObjects/particle_initializer/particles_per_element={ppe:d}",
             f"Outputs/file_base={file_base}",
             ]

        mooseutils.run_executable(
            executeable, *a, mpi=mpi, suppress_output=supress_output)
