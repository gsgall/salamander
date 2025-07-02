import mooseutils
import os
from random import seed, randint
import glob


input_file = "./testing.i"
mpi=10
executeable="../salamander-opt"
supress_output = False

seed(0)

particles_per_element = [400, 1000]

for ppe in particles_per_element:

  file_path = f"relax_study/100x100/{ppe:d}"
  if not os.path.exists(file_path, ):
      os.makedirs(file_path)
  for i in range(10):
    print(f"Relaxation run:{i:d}")
    folder = f"{file_path:s}/run{i:d}"

    if not os.path.exists(folder):
      os.makedirs(folder)
    a = ["-i", input_file, "--allow-test-objects", f"seed={randint(0, int(1e8))}", f"particles_per_element={ppe:d}"]

    mooseutils.run_executable(executeable, *a, mpi=mpi, suppress_output=supress_output)
    files = glob.glob(f'*.csv', root_dir="./", recursive=False)
    for f in files:
      os.rename(f"{f:s}", f"{folder:s}/{f:s}")
