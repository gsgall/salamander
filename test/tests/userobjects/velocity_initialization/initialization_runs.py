import mooseutils
import os
import unittest
from random import seed, randint
import pandas as pd
import numpy as np


def actualVSExpectedVariance(actual_mean, data, particles_averaged):
    num_samples = len(data)
    measured_mean = data.mean()
    error_mean = np.abs(
        measured_mean - actual_mean) / actual_mean

    measured_relative_var = np.sqrt(
        ((data - actual_mean)**2).sum() / num_samples) / actual_mean
    expected_relative_var = np.sqrt(2 / (3 * particles_averaged))
    error_var = np.abs(
        measured_relative_var - expected_relative_var) / expected_relative_var

    return np.array([measured_mean, error_mean, measured_relative_var,
                     error_var]).reshape((1, 4))


class MaxwellianInitializationTest(unittest.TestCase):
    def test(self):
        seed(0)

        supress_output = True

        # find the version of the executable that was built
        method = os.environ.get("METHOD", "opt")
        mpi = 1
        executeable = "../../../../salamander-" + method

        num_runs = 40
        all_data = None

        for i in range(num_runs):
            print(f"Run {i}")

            a = ["-i", "maxwellian.i", "--allow-test-objects",
                 f"GlobalParams/seed={randint(0, int(1e8))}",
                 "Outputs/file_base=multi_run"
                 ]

            mooseutils.run_executable(executeable, *a, mpi=mpi,
                                      suppress_output=supress_output)

            data = pd.read_csv("multi_run_element_temperatures_0001.csv")
            if i == 0:
                all_data = data["temperature"].to_numpy()
                continue
            temporary_data = data["temperature"].to_numpy()

            all_data = np.concatenate((all_data, temporary_data))

        np.savetxt("maxwellian_temperatures.csv", all_data,
                   header='temperature', fmt='%0.4E', comments='')

        values_and_errors = actualVSExpectedVariance(
            273.15, all_data, particles_averaged=10)

        header = "temperature,temperature_error,relative_variance,variance_error"
        np.savetxt("maxwellian_values_and_errors.csv", values_and_errors,
                   header=header, delimiter=',', comments='', fmt='%0.4E')


if __name__ == "__main__":
    MaxwellianInitializationTest.test()
