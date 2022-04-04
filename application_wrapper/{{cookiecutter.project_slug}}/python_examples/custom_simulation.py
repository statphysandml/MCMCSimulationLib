import numpy as np


""" Example for accessing and executing C++ functions of the MCMCSystem in Python.

Since the MCMC system class inherits from the MCMCSystem class, support of the C++ functions
is automatically taken care of """


if __name__ == "__main__":
    # Setting up the system
    from {{ cookiecutter.project_slug }}.{{ cookiecutter.project_slug }} import {{ cookiecutter.project_name }}
    mcmc_system = {{ cookiecutter.project_name }}(mu=[1.0, 2.0, -1.5], sigma=1.0, dt=0.002)

    mcmc_system.initialize("hot")
    print("System size:", mcmc_system.size())

    mcmc_system.measure_names = ["Mean", "SecondMoment"]
    print("Measure name:", mcmc_system.measure_names)

    print("Measurements:", mcmc_system.measure(), "Random variable for mu = 1.0:", mcmc_system[0])
    mcmc_system.update(n_step=100)
    print("Measurements:", mcmc_system.measure(), "Random variable for mu = 1.0:", mcmc_system[0])

    print("Random variables:", mcmc_system.system_repr)
    mcmc_system.system_repr = np.zeros_like(mcmc_system.system_repr)
    print("Reset random variables: ", mcmc_system.system_repr)
    print("Measurements:", mcmc_system.measure(), "Random variable for mu = 1.0:", mcmc_system[0])
