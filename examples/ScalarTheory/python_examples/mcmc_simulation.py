from pystatplottools.pdf_env.loading_figure_mode import loading_figure_mode
fma, plt = loading_figure_mode(develop=True)


""" Example for performing simulations based on the MCMCSimulation class from the MCMCEvaluationLib

The MCMCSimulation class supports the same execution modes as the MCMCSimulationLib. Respectively,
it is possible to perform an entire simulation also with this class. However, note that this is
slower than performing the simulation solely in C++ """


if __name__ == "__main__":
    from scalartheory.scalar_theory import ScalarTheory
    scalar_theory = ScalarTheory(kappa=0.23, lambd=0.02, dimensions=[4, 4], dt=0.14, n=10, m=1.0)

    running_parameter = "kappa"
    rp_values = [0.23, 0.24, 0.25]

    from mcmctools.mcmc.mcmc_simulation import MCMCSimulation
    simulation = MCMCSimulation(model=scalar_theory,
                                running_parameter=running_parameter,  # Currently only supported for model parameters
                                rp_values=rp_values)

    # Equilibrium time simulation and evaluation
    simulation.run_equilibrium_time_simulation(measure="SecondMoment", sample_size=40, number_of_steps=200)
    data = simulation.measurements_to_dataframe()

    from mcmctools.modes.equilibrium_time import equilibrium_time
    equilibrium_times = equilibrium_time(
        sample_size=40, number_of_steps=200,
        confidence_range=0.1, confidence_window=10, measure="SecondMoment",
        running_parameter=running_parameter, rp_values=rp_values,
        data=data, fma=fma)
    print(equilibrium_times)

    # Correlation time simulation and evaluation
    simulation.run_correlation_time_simulation(measure="SecondMoment", minimum_sample_size=100,
                                               maximum_correlation_time=80, start_measuring=1000,
                                               starting_mode="cold")
    data = simulation.measurements_to_dataframe()

    from mcmctools.modes.correlation_time import correlation_time
    correlation_times = correlation_time(
        minimum_sample_size=100, maximum_correlation_time=80, measure="SecondMoment",
        running_parameter=running_parameter, rp_values=rp_values,
        data=data, fma=fma)
    print(correlation_times)

    # Expectation value simulation and evaluation
    simulation.run_expectation_value_simulation(measures=["Mean", "Config"], n_measurements=1000,
                                                n_steps_equilibrium=100, n_steps_autocorrelation=10,
                                                starting_mode="cold")
    data = simulation.measurements_to_dataframe()

    from mcmctools.modes.expectation_value import expectation_value
    expectation_values = expectation_value(
        number_of_measurements=1000, measures=["Mean", "Config"],
        error_type="statistical",
        running_parameter=running_parameter, rp_values=rp_values,
        data=data)
    print(expectation_values)

    print(data)
