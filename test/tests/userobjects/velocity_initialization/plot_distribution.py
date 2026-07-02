#!/usr/bin/env python
import os
import sys
import numpy as np
import pandas as pd
from scipy import stats
import matplotlib.pyplot as plt

plt.rcParams['mathtext.fontset'] = 'stix'
plt.rcParams['font.family'] = 'STIXGeneral'
plt.rcParams['legend.frameon'] = False

font = 18
plt.rcParams['legend.fontsize'] = font-2
plt.rcParams['axes.titlesize'] = font
plt.rcParams['axes.labelsize'] = font

plt.rcParams['xtick.labelsize'] = font-1
plt.rcParams['ytick.labelsize'] = font-1

# overall figure
plt.rcParams['figure.frameon'] = False

# Resetting default matplotlib parameters
linewidth = 1.5
plt.rcParams['lines.linewidth'] = 3

# axes
plt.rcParams['axes.linewidth'] = linewidth
plt.rcParams['axes.titlesize'] = font
plt.rcParams['axes.labelsize'] = font
plt.rcParams['axes.labelpad'] = 4  # default is 4

# ticks
plt.rcParams['xtick.direction'] = 'in'
plt.rcParams['ytick.direction'] = 'in'
plt.rcParams['xtick.labelsize'] = font-1
plt.rcParams['ytick.labelsize'] = font-1
plt.rcParams['lines.markersize'] = 8

plt.rcParams['xtick.major.size'] = 10 / 1.5  # Length of major ticks on x-axis
plt.rcParams['xtick.minor.size'] = 5 / 1.5  # Length of minor ticks on x-axis
plt.rcParams['ytick.major.size'] = 10 / 1.5  # Length of major ticks on y-axis
plt.rcParams['ytick.minor.size'] = 5 / 1.5  # Length of minor ticks on y-axis

plt.rcParams['xtick.major.width'] = linewidth / 1.5
plt.rcParams['ytick.major.width'] = linewidth / 1.5
plt.rcParams['xtick.minor.width'] = linewidth / 1.5
plt.rcParams['ytick.minor.width'] = linewidth / 1.5

plt.rcParams['xtick.top'] = True
plt.rcParams['xtick.bottom'] = True
plt.rcParams['ytick.left'] = True
plt.rcParams['ytick.right'] = True

# legend
plt.rcParams['legend.frameon'] = False


def histogramFromData(data, bins=25):
    hist = np.histogram(data, bins=bins, density=True)
    values = hist[0]
    centers = (hist[1][1:] + hist[1][:-1]) / 2.0
    widths = hist[1][1:] - hist[1][:-1]
    return values, centers, widths


# Changes working directory to script directory (for consistent MooseDocs usage)
script_folder = os.path.dirname(__file__)
os.chdir(script_folder)

if "/salamander/doc/" in script_folder.lower():
    data_folder = "../../../../../test/tests/userobjects/velocity_initialization/"
else:
    data_folder = "./"

gold_file = data_folder + "gold/maxwellian_temperatures.csv"


temperature_data = pd.read_csv(gold_file).to_numpy()
values, centers, widths = histogramFromData(temperature_data)

T_0 = 273.15  # initialized temperature in K
temperature_range = np.linspace(50, 700, 1000)
particles_per_element = 10
_, ax = plt.subplots()
ax.bar(centers, values * 1e3, widths, edgecolor='k', label='Samples')
ax.plot(temperature_range,
        stats.chi2.pdf(temperature_range, df=3 * particles_per_element,
                       scale=T_0 / (3 * particles_per_element)) * 1e3,
        '-k', label='$\\chi^2 \\left(3 N_p,\\frac{T_0}{3 N_p}\\right)$')

ax.set_xlabel("Temperature (K)")
ax.set_ylabel("PDF (K$^{-1}$)")
ax.text(0.11, 1.01, "$\\times10^{-3}$", transform=ax.transAxes,
        fontsize=12, ha='right', va='bottom')
ax.legend(frameon=False)
plt.savefig('maxwellian_temperature_distribution.png',
            format='png', bbox_inches='tight', dpi=300)
