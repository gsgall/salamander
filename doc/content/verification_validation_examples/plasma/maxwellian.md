# Velocity Initialiazation

While performing particle-in-cell simulations a useful initial condition for velocity space is to initialize particles at a specific temperature with velocities sampled from a Maxwellian distribution.
This is also a very useful velocity space initial condition for collisional verification problems.
Before proceeding with those sort problems it is important to first demonstrate that the implementation of the initial condition on which those cases are based is correct.

# Theory

Temperature is a measure of the mean energy in a system.
In this exercise we adopt the same definition of temperature as Liebermann  TODO: add citation
For three dimensinoal system the temperature is computed as

\begin{equation}
    \frac{1}{2} m \langle v^2 \rangle = \frac{1}{2} k_B T
\end{equation}

The velocity distribution of a single component that is Maxwellian distributed is given by

\begin{equation}
  f_{v_i} \left( v_i \right) =
  \sqrt{ \frac{ m }{ 2 \pi k_B T }}
  \exp \left( - \frac{ m v_i^2 }{ 2 k_B T_i } \right)
\end{equation}

where $i$ denotes the component of the velocity vector, $m$ is the mass of the species, and $T$ is the temperature in Kelvin.
The key thing to note is that this distribution is simply a special case of the normal ditribution

\begin{equation}
  \mathcal{N} (\mu, \sigma^2) =
  \frac{ 1 }{ \sqrt{2 \pi \sigma^2} }
  \exp \left( - \frac{ \left( x - \mu \right)^2 }{ 2 \sigma^2 } \right).
\end{equation}
The two are equivalent if the mean,
\begin{equation}
  \mu = 0,
\end{equation}
and the variance $\sigma^2$
\begin{equation}
  \sigma^2 = \frac{ k_B T_i }{ m }.
\end{equation}

At this point it is important to note that the variance of a normal distribution is distributed like a $\chi^2$ distribution.
Using this fact and the fact that the expection of several moments of the normal are given by

\begin{equation}
  E \left[ x \right] = 0,
\end{equation}
\begin{equation}
  E \left[ x^2 \right] = \sigma^2,
\end{equation}
\begin{equation}
  E \left[ x^4 \right] = 3 \sigma^4,
\end{equation}

we can derrive an explict formula for the variance of the temperature of a single velocity component.
This is given by

\begin{equation}
  \text{Var}\left( T \right)
  =
  \frac{ 2 m^2 \sigma^4 }{ k_B^2 N_p }
\end{equation}

Where $N_p$ is the number of particles used to compute the temperature of particles.
And if all three components of velocity have the same temperature then the variance of the temperature is reduced further by a factor of one third.

\begin{equation}
  \text{Var}\left( T \right)
  =
  \frac{ 2 m^2 \sigma^4 }{ 3 k_B^2 N_p }
\end{equation}

# Verificiation

The previous analysis yields two points of comparison for verification of this particle implementation.
The analysis provides an analytically computable value for the expected variance in the temperature.
If the temperature is computed on an element by elment basis for a fixed number of particles per element and many indepedent samples are collected then the variance of the temperature should match the theoritical value closely.
Additionally, as more and more samples are collected the value of the variance should converge to the theoretically predicted value.
The second point of comparison is that of the distribution of the temperature in each element.
Since the number of particles per element is known then the distribution of the temperature for a fixed number of particles per element is also known and if plotted together they should agree.

!media plot_distribution.py
       image_name=maxwellian_temperature_distribution.png
       style=margin-left:auto;margin-right:auto;width:50%;
       id=per_elment_temperature_dists
       caption=The distribution of calculated temperatures in each element. The initialized temperatures follow the expected distribution.

# Running the Case

To run this input, use the following commands in your Terminal. The `--allow-test-objects` flag allows the use of custom SALAMANDER testing objects that are not used for "production" physics capability, but are useful for creating simple cases such as this one.

!alert note title=MOOSE Tools.
This case relies on functionality of the MOOSE Tools python package. Make sure those tools are added to your PYTHONPATH before proceeding. If you are using MOOSE as a submodule of SALAMANDER then you make run the following command.

```bash
export PYTHONPATH=$PYTHONPATH:~/projects/salamander/moose/python

```

```bash
  cd ~/projects/salamander/test/tests/userobjects/velocity_initialization/
  python initialization_runs.py
  python plot_distribution.py
```
