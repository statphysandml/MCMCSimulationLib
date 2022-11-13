from pystatplottools.pdf_env.loading_figure_mode import loading_figure_mode
fma, plt = loading_figure_mode(develop=False)


""" Example for executing a full MCMC simulation in C++ by using the pybind11 modules. This is the
recommended way to use the pybind11 modules in Python. The performed simulation is the same as the one in the
main.cpp file. """


if __name__ == "__main__":
    # Ensure to be in the same directory as the file
    import os
    dir_path = os.path.dirname(os.path.realpath(__file__))
    os.chdir(dir_path)

    # Directory for identifying the simulation
    target_name = "{{ cookiecutter.project_name }}ModeSimulation"

    # Setting up the system
    from {{ cookiecutter.project_slug }}.{{ cookiecutter.project_slug }} import {{ cookiecutter.project_name }}
    mcmc_system = {{ cookiecutter.project_name }}(mu=[1.0, 2.0, -1.5], sigma=1.0, dt=0.002)

    # The simulation is executed several times with changing parameters of kappa, as running_parameter
    from mcmctools.utils.utils import linspace_rp_intervals
    rp_values = linspace_rp_intervals(0.2, 1.0, 9)

    """ Setting up and running the simulation with the ModeSimulation class """

    from mcmc.mode_simulation import ModeSimulation
    mode_simulation = ModeSimulation(model=mcmc_system,
                                     sim_base_dir="./../",
                                     rel_data_dir="data/" + target_name + "/",
                                     rel_results_dir="results/" + target_name + "/",
                                     running_parameter_kind="systembase", running_parameter="sigma",
                                     rp_values=rp_values
                                     )

    # Equilibrium time simulation
    mode_simulation.equilibrium_time_simulation(run=True, eval=True, sample_size=20, number_of_steps=2000,
                                                eval_confidence_range=0.1,
                                                eval_confidence_window=10, measure="Mean")

    # Correlation time simulation
    mode_simulation.correlation_time_simulation(run=True, eval=True, minimum_sample_size=2000,
                                                maximum_correlation_time=400,
                                                equilibrium_time_rel_results_dir="results/" + target_name + "/",
                                                measure="Mean",
                                                starting_mode="hot")

    # Expectation value simulation
    mode_simulation.expectation_value_simulation(run=True, eval=True,
                                                 correlation_time_rel_results_dir="results/" + target_name + "/",
                                                 number_of_measurements=100000,
                                                 equilibrium_time_rel_results_dir="results/" + target_name + "/",
                                                 measures=["Config", "Mean"],
                                                 starting_mode="hot"
                                                 )

    # Since the ModeSimulation class inherits from the EvaluationModule class, it also supports all of its features,
    # as for example:

    # Loading the MCMC Simulation data of the expectation value simulation
    data = mode_simulation.load_data(identifier="expectation_value")
    sim_params = mode_simulation.load_sim_params(identifier="expectation_value")

    # Loading the expectation value results
    mode_simulation.load_expectation_values()
    print(mode_simulation.expectation_values)
