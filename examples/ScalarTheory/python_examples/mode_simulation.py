from pystatplottools.pdf_env.loading_figure_mode import loading_figure_mode

fma, plt = loading_figure_mode(develop=False)

from mcmc.mode_simulation import ModeSimulation
from scalartheory.scalar_theory import ScalarTheory


if __name__ == "__main__":
    import os

    dir_path = os.path.dirname(os.path.realpath(__file__))
    os.chdir(dir_path)

    scalar_theory = ScalarTheory(kappa=0.24, lambd=0.02, dimensions=[4, 4], dt=0.01, n=100, m=1)

    from mcmctools.utils.utils import linspace_rp_intervals
    rp_values = linspace_rp_intervals(0.22, 0.3, 9)

    mode_simulation = ModeSimulation(model=scalar_theory,
                                     sim_base_dir="./",
                                     rel_data_path="./data/Test/",
                                     rel_results_path="./data/Test/results/",
                                     running_parameter_kind="systembase_params", running_parameter="kappa",
                                     rp_values=rp_values
                                     )

    mode_simulation.equilibrium_time_simulation(run=True, eval=True, sample_size=40, number_of_steps=10,
                                                eval_confidence_range=0.1,
                                                eval_confidence_window=10, measure="Mean")

    mode_simulation.correlation_time_simulation(run=True, eval=True, minimum_sample_size=10,
                                                maximum_correlation_time=200,
                                                equilibrium_time_rel_results_path="./data/Test/results/", measure="Mean"
                                                )

    mode_simulation.compute_correlation_time(from_file=True, fma=fma)

    mode_simulation.expectation_value_simulation(run=True, eval=False,
                                                 correlation_time_rel_results_path="./data/Test/results/",
                                                 number_of_measurements=1000,
                                                 equilibrium_time_rel_results_path="./data/Test/results/",
                                                 measures=["Mean", "Config"]
                                                 )

    mode_simulation.compute_expectation_value(from_file=True)  # measures=["Mean", "SecondMoment"])

    data, sim_params = mode_simulation.load_data_from_file()

    # mode_simulation.load_expectation_values_from_file()
    print(mode_simulation.expectation_values)

