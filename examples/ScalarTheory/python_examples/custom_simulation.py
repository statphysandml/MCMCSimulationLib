from pystatplottools.pdf_env.loading_figure_mode import loading_figure_mode
fma, plt = loading_figure_mode(develop=False)

from scalartheory.scalar_theory import ScalarTheory
from mcmctools.mcmc.mcmc_simulation import MCMCSimulation


if __name__ == "__main__":
    scalar_theory = ScalarTheory(kappa=0.24, lambd=0.02, dimensions=[4, 4], dt=0.01, n=100, m=1)

    scalar_theory.initialize("hot")

    simulation = MCMCSimulation(model=scalar_theory,
                                rel_data_path="./data/Test/",
                                rel_results_path="./data/Test/results/",
                                # running_parameter_kind="model_params",
                                running_parameter="kappa",
                                rp_values=[0.1, 0.4, 0.7])

    simulation.run_equilibrium_time_simulation(measure="Mean", sample_size=40, number_of_steps=100)

    data = simulation.measurements_to_dataframe()
    simulation.compute_equilibrium_time(data=data, sample_size=40, number_of_steps=100, eval_confidence_range=0.1,
                                        eval_confidence_window=10, measure="Mean", fma=fma)

    # simulation.run_expectation_value_simulation(
    #     measures=["Mean", "Config"], n_measurements=1000, n_steps_equilibrium=100, n_steps_autocorrelation=100)
    #
    # data = simulation.measurements_to_dataframe()
    # simulation.compute_expectation_value(measures=["Mean", "Config"],
    #     data=data, eval_n_means_bootstrap=0)  # measures=["Mean", "SecondMoment"])
    print(1)
    # simulation.load_config_data()
