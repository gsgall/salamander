import os
import unittest

import matplotlib.pyplot as plt
import mooseutils
import numpy as np
import pandas as pd
from numpy.polynomial import Polynomial


class TestTwoStream(unittest.TestCase):

    def test(self):
        # find the version of the executable that was built
        method = os.environ.get("METHOD", "opt")
        mpi = 1
        # setting the path for the executable depending on whether or not we are running a test
        executeable = "../../../../salamander-" + method

        args = [
            "-i",
            "two-stream.i",
            "particles_per_element=10",
            "num_elems=8",
            "VectorPostprocessors/rays/enable=False",
            "--allow-test-objects",
        ]
        mooseutils.run_executable(executeable, *args, mpi=mpi)

        ts_data = pd.read_csv("two-stream_out.csv")
        ts_fitting_times = ts_data["time"].to_numpy().reshape((-1, 1))
        ts_start_time = 10
        ts_stop_time = 23
        # ts_mid_point = (ts_start_time + ts_stop_time) / 2
        ts_start_idx = np.where(ts_fitting_times >= ts_start_time)[0][0]
        ts_stop_idx = np.where(ts_fitting_times >= ts_stop_time)[0][0]

        ts_fitting_times = ts_data["time"][ts_start_idx:ts_stop_idx]
        ts_fitting_data = np.log(
            ts_data["Emax"][ts_start_idx:ts_stop_idx] / ts_data["Emax"][1]
        )

        ts_fit = Polynomial.fit(ts_fitting_times, ts_fitting_data, deg=1).convert().coef

        ts_k = 1.0  # rad / m
        ts_v_0 = 1.0  # m / s
        ts_n_sim = 1.0  # 1 / m^3
        ts_mass = 1.0  # kg
        ts_charge = -1.0  # C
        ts_eps0 = 1.0  # C^2 s^2 / (kg  m^3)
        ts_omega_p_sim = np.sqrt(ts_n_sim * ts_charge**2 / (ts_eps0 * ts_mass))  # rad

        ts_analytic_omega = np.sqrt(
            -(
                (ts_k * ts_v_0) ** 2
                + ts_omega_p_sim**2
                - ts_omega_p_sim
                * np.sqrt(4.0 * (ts_k * ts_v_0) ** 2.0 + ts_omega_p_sim**2)
            )
        )
        ts_simulated_omega = ts_fit[1]
        percent_error_growth_rate = (
            np.abs(ts_analytic_omega - ts_simulated_omega) / ts_analytic_omega * 100
        )

        self.assertLessEqual(percent_error_growth_rate, 4.0)

        # print(f"Analytic  Growth Rate: {ts_analytic_omega:0.4E}")
        # print(f"Simulated Growth Rate: {ts_simulated_omega:0.4E}")
        # print(f"        Percent Error: {ts_error * 100:0.4E}%")
        #


if __name__ == "__main__":
    unittest.main(__name__, verbosity=2)
