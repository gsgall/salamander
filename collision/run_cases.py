import glob
import math
import os
from random import randint, seed

import mooseutils

input_file = "./testing.i"
mpi = 8
executeable = "../salamander-opt"
supress_output = True

seed(0)

runs = 2


np_changes = 6
max_runs = 10**4
# min number of particles per cell
min_np = 5

ppe = [5, 10, 20, 40, 80, 160]
T_alpha_0 = [1444, 1004, 1127, 1054, 1045, 974]
T_beta_0 = [145, 101, 113, 106, 105, 96]

for i in range(np_changes):
    runs = int(max_runs / (math.sqrt(2) ** i))
    ppe = min_np * 2**i

    main_folder = f"{ppe:d}_ppe"
    os.makedirs(main_folder, exist_ok=True)
    print(f"{ppe:d} PPE")

    for j in range(runs):
        print(f"Run:{j:d}")

        a = [
            "-i",
            input_file,
            "--allow-test-objects",
            f"particles_per_element={ppe:d}",
            f"seed={randint(0, int(1e8))}",
            f"Outputs/file_base={main_folder}/run_{j:d}",
            f"T_alpha={T_alpha_0[i]:d}",
            f"T_beta={T_beta_0[i]:d}",
        ]
        mooseutils.run_executable(
            executeable, *a, mpi=mpi, suppress_output=supress_output
        )
