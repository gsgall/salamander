import mooseutils
import os
from random import seed, randint
import glob


supress_output = True

# find the version of the executable that was built
method = os.environ.get("METHOD", "opt")
mpi = 1
# setting the path for the executable depending on whether or not we are running a test
executeable = "../salamander-" + method

if "test" not in os.getcwd():
    executeable = "../salamander-" + method

seed(0)

for i in range(40):
  print(f"Run {i}")
  file_path = "data"
  if not os.path.exists(file_path):
      os.makedirs(file_path)

  a = ["-i", "testing.i", "--allow-test-objects", f"GlobalParams/seed={randint(0, int(1e8))}", f"Outputs/file_base={file_path}/run_{i}"]

  mooseutils.run_executable(executeable, *a, mpi=mpi, suppress_output=supress_output)
