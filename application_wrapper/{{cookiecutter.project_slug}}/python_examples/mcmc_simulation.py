from pystatplottools.pdf_env.loading_figure_mode import loading_figure_mode
fma, plt = loading_figure_mode(develop=True)


""" Example for performing simulations based on the MCMCSimulation class from the MCMCEvaluationLib

The MCMCSimulation class supports the same execution modes as the MCMCSimulationLib. Respectively,
it is possible to perform an entire simulation also with this class. However, note that this is
slower than performing the simulation solely in C++ """


if __name__ == "__main__":
    # Setting up the system
    from {{ cookiecutter.project_slug }}.{{ cookiecutter.project_slug }} import {{ cookiecutter.project_name }}
    mcmc_system ={{ cookiecutter.project_name }}(mu=[1.0, 2.0, -1.5], sigma=1.0, dt=0.002)

    running_parameter = "sigma"
    rp_values = [0.1, 0.2, 0.23]

    from mcmctools.mcmc.mcmc_simulation import MCMCSimulation
    simulation = MCMCSimulation(model=mcmc_system,
                                running_parameter=running_parameter,  # Currently only supported for model parameters
                                rp_values=rp_values)

    # Equilibrium time simulation and evaluation
    simulation.run_equilibrium_time_simulation(measure="Mean", sample_size=40, number_of_steps=200)
    data = simulation.measurements_to_dataframe()

    from mcmctools.modes.equilibrium_time import equilibrium_time
    equilibrium_times = equilibrium_time(
        sample_size=40, number_of_steps=200,
        confidence_range=0.1, confidence_window=10, measure="Mean",
        running_parameter=running_parameter, rp_values=rp_values,
        data=data, fma=fma)
    print(equilibrium_times)

    # Correlation time simulation and evaluation
    simulation.run_correlation_time_simulation(measure="Mean", minimum_sample_size=100,
                                               maximum_correlation_time=200, start_measuring=1000,
                                               starting_mode="hot")
    data = simulation.measurements_to_dataframe()

    from mcmctools.modes.correlation_time import correlation_time
    correlation_times = correlation_time(
        minimum_sample_size=100, maximum_correlation_time=200, measure="Mean",
        running_parameter=running_parameter, rp_values=rp_values,
        data=data, fma=fma)
    print(correlation_times)

    # Expectation value simulation and evaluation
    simulation.run_expectation_value_simulation(measures=["Mean", "Config"], n_measurements=100000,
                                                n_steps_equilibrium=100, n_steps_autocorrelation=10,
                                                starting_mode="hot")
    data = simulation.measurements_to_dataframe()

    from mcmctools.modes.expectation_value import expectation_value
    expectation_values = expectation_value(
        number_of_measurements=100000, measures=["Mean", "Config"],
        error_type="statistical",
        running_parameter=running_parameter, rp_values=rp_values,
        data=data)
    print(expectation_values)

    print(data)
