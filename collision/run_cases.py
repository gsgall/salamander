import glob
import os
from random import randint, seed

import mooseutils

input_file = "./testing.i"
mpi = 8
executeable = "../salamander-opt"
supress_output = True

seed(0)

runs = 2
for i in range(runs):
    print(f"Run:{i:d}")

    a = [
        "-i",
        input_file,
        "--allow-test-objects",
        f"seed={randint(0, int(1e8))}",
        f"Outputs/file_base=run_{i:d}",
    ]

    mooseutils.run_executable(executeable, *a, mpi=mpi, suppress_output=supress_output)
