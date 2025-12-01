#!/usr/bin/env python
import os
import random

import mooseutils

input_path = "test/tests/userobjects/particle_initializer/per_element/elements"
mpi = 10
executeable = "./salamander-opt"
supress_output = False


inputs = {
    # 1: ["EDGE2"],
    # 2: ["QUAD4", "TRI3"],
    # 3: ["HEX8", "PRISM6", "PYRAMID4", "PYRAMID5", "TET4"]
    3: ["TET4"]
}

elems_per_dim = [100, 10, 5]
directions = ["x", "y", "z"]

uniform_refines = list(range(4))

num_trials = 10

for dim, elems in inputs.items():
    dim_dir = f"{dim:d}D"
    os.makedirs(dim_dir, exist_ok=True)

    charge_density = 2 * dim

    args = ["--allow-test-objects", f"charge_density={charge_density:d}"]

    for i in range(dim):
        args.append(f"Mesh/gmg/n{directions[i]:s}={elems_per_dim[dim-1]}")

    args.append("-i")

    for elem in elems:
        elem_dir = f"{dim_dir}/{elem:s}"
        os.makedirs(elem_dir, exist_ok=True)

        input_file = f"{input_path:s}/{dim}d.i"

        args2 = args.copy()
        args2.append(input_file)
        args2.append(f"Mesh/gmg/elem_type={elem:s}")

        study_dir = f"{elem_dir:s}/elem_refine"
        os.makedirs(study_dir, exist_ok=True)

        # elemental refinements constant particles per element

        particles_per_element = 200

        if dim == 1:
            particles_per_element = 1000

        args2.append(f"particles_per_element={particles_per_element:d}")
        for i in uniform_refines:
            refine_dir = f"{study_dir:s}/refine_{i:d}"
            os.makedirs(refine_dir, exist_ok=True)
            for j in range(num_trials):
                args3 = args2.copy()
                args3.append(f"Mesh/uniform_refine={i:d}")
                random.seed(i + j)
                args3.append(
                    f"UserObjects/particle_initializer/seed={random.randint(0, 123540):d}"
                )
                file_base = f"{refine_dir}/run_{j:d}"
                args3.append(f"Outputs/file_base={file_base:s}")
                mooseutils.run_executable(
                    executeable, *args3, mpi=mpi, suppress_output=supress_output
                )

        args2 = args.copy()
        args2.append(input_file)
        args2.append(f"Mesh/gmg/elem_type={elem:s}")

        # particle refinements study
        study_dir = f"{elem_dir:s}/particle_refine"
        os.makedirs(study_dir, exist_ok=True)

        for i in uniform_refines:
            refine_dir = f"{study_dir:s}/refine_{i:d}"
            os.makedirs(refine_dir, exist_ok=True)
            for j in range(num_trials):
                args3 = args2.copy()
                args3.append(f"particles_per_element={10 ** (i + 1):d}")
                random.seed(i + j)
                args3.append(
                    f"UserObjects/particle_initializer/seed={random.randint(0, 123540):d}"
                )
                file_base = f"{refine_dir}/run_{j:d}"
                args3.append(f"Outputs/file_base={file_base:s}")
                mooseutils.run_executable(
                    executeable, *args3, mpi=mpi, suppress_output=supress_output
                )
