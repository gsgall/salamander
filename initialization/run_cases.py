import mooseutils
import os
from random import seed, randint
import glob


input_file = "./testing.i"
mpi = 6
executeable = "../salamander-opt"
supress_output = True

seed(0)

for i in range(100):
  print(f"Run {i}")
  file_path = "data"
  if not os.path.exists(file_path):
      os.makedirs(file_path)

  a = ["-i", input_file, "--allow-test-objects", f"GlobalParams/seed={randint(0, int(1e8))}", f"Outputs/file_base={file_path}/run_{i}"]

  mooseutils.run_executable(executeable, *a, mpi=mpi, suppress_output=supress_output)
