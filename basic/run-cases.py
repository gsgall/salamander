import mooseutils
import os


mpi = 1
executeable = "../salamander-opt"
supress_output = False

ppe_vals = [10 * 2**(i) for i in range(5)]

single_file_path = "./single_data"
if not os.path.exists(single_file_path):
    os.makedirs(single_file_path)

multi_file_path = "./multi_data"
if not os.path.exists(multi_file_path):
    os.makedirs(multi_file_path)


for ppe in ppe_vals:
    print(f"ppe: {ppe}")
    file_base = f"{single_file_path:s}/{ppe:d}_ppe"
    a = ["-i", "single-reaction.i",
         "--allow-unused",
         "--allow-test-objects",
         f"Outputs/file_base={file_base}",
         f"GlobalParams/particles_per_element={ppe:d}",
         "Outputs/console=true",
         "Outputs/exodus=false"
         ]

    mooseutils.run_executable(
        executeable, *a, mpi=mpi, suppress_output=supress_output)
