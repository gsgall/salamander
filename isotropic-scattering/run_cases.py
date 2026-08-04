import mooseutils
import os
from random import seed, randint


input_file = "./testing.i"
mpi = 10
executeable = "../salamander-opt"
supress_output = False

seed(0)

elems_per_dim = 10
runs = 10
particles_per_element = [400]
components = ['x', 'y', 'z']

for ppe in particles_per_element:
    folder = f"hard_sphere/{elems_per_dim}x{
        elems_per_dim}/distribution/{ppe:d}_ppe"
    if not os.path.exists(folder):
        os.makedirs(folder)
    for i, component in enumerate(components):
        file_path = f"{folder}/{component}"
        distributions = ""
        for j in range(3):
            if i == j:
                distributions += "v_0 "
            else:
                distributions += "v_12 "
        print(distributions)
        for j in range(runs):
            print(f"Run: {j}")
            file_base = f"{file_path:s}/run_{j:d}"
            a = ["-i", input_file, "--allow-unused",
                 "--allow-test-objects",
                 f"GlobalParams/seed={randint(0, int(1e8))}",
                 f"particles_per_element={ppe:d}",
                 f"Outputs/file_base={file_base}",
                 "Outputs/console=false",
                 f"UserObjects/velocity_initializer/distributions='{
                     distributions}'"
                 ]

            mooseutils.run_executable(
                executeable, *a, mpi=mpi, suppress_output=supress_output)
