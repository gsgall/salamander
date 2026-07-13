import mooseutils
import os
from random import seed, randint


input_file = "./testing.i"
mpi = 10
executeable = "../salamander-opt"
supress_output = False

seed(0)

elems_per_dim = 10
runs = 30
particles_per_element = [400]

for ppe in particles_per_element:
    file_path = f"maxwell/{elems_per_dim}x{elems_per_dim}/{ppe:d}_ppe"
    if not os.path.exists(file_path):
        os.makedirs(file_path)
    for i in range(runs):
        print(f"Run: {i}")
        file_base = f"{file_path:s}/run_{i:d}"
        a = ["-i", input_file, "--allow-unused",
             "--allow-test-objects",
             f"GlobalParams/seed={randint(0, int(1e8))}",
             f"particles_per_element={ppe:d}",
             f"Outputs/file_base={file_base}",
             "Outputs/console=false"]

        mooseutils.run_executable(
            executeable, *a, mpi=mpi, suppress_output=supress_output)
