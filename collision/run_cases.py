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


ppe = [2**i for i in range(3, 9)]
runs =[2**i for i in range(10, 4, -1)]

total_fails = 0
for N_p, N_s in zip(ppe, runs):
    main_folder = f"{N_p:d}_ppe"
    os.makedirs(main_folder, exist_ok=True)
    print(f"{N_p:d} PPE")

    for j in range(N_s):
        print(f"Run:{j:d}")
        general_seed = randint(0, int(1e8))
        alpha_seed = randint(0, int(1e8))
        beta_seed = randint(0, int(1e8))
        a = [
            "-i",
            input_file,
            "--allow-test-objects",
            f"particles_per_element={N_p:d}",
            f"seed={general_seed:d}",
            f"alpha_seed={alpha_seed:d}",
            f"beta_seed={beta_seed:d}",
            f"Outputs/file_base={main_folder}/run_{j:d}",
        ]
        exit_code = mooseutils.run_executable(
            executeable, *a, mpi=mpi, suppress_output=supress_output
        )

        if exit_code != 0:
            total_fails += 1
            print("Simulation Failure")
            print(f"Current fail count: {total_fails:d}")


print("All Simulations Complete")
print(f"Simulations Failed {total_fails:d}")
