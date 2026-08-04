import mooseutils
import os


input_file = "./testing.i"
mpi = 1
executeable = "../salamander-opt"
supress_output = False


file_path = "./data"
if not os.path.exists(file_path):
    os.makedirs(file_path)

# ppe_vals = [30 * 2**(i) for i in range(5)]
# ppe_vals = [10**(i + 1) for i in range(5)]
#
# for ppe in ppe_vals:
#    print(f"PPE: {ppe}")
#    file_base = f"{file_path:s}/{ppe:d}_ppe"
#    a = ["-i", input_file,
#         "--allow-unused",
#         "--allow-test-objects",
#         f"Outputs/file_base={file_base}",
#         f"GlobalParams/particles_per_element={ppe:d}",
#         "Outputs/console=true"
#         ]
#
#    mooseutils.run_executable(
#        executeable, *a, mpi=mpi, suppress_output=supress_output)
#

dt_vals = [0.5**i for i in range(10)]
for dt in dt_vals:
    print(f"dt: {dt:0.4f}")
    file_base = f"{file_path:s}/dt_{dt:0.4f}"

    a = ["-i", input_file,
         "--allow-unused",
         "--allow-test-objects",
         f"Outputs/file_base={file_base}",
         f"GlobalParams/particles_per_element={10000:d}",
         f"Executioner/dt={dt:0.4f}",
         "Outputs/console=true"
         ]

    mooseutils.run_executable(
        executeable, *a, mpi=mpi, suppress_output=supress_output)
