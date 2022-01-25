from pystatplottools.pdf_env.loading_figure_mode import loading_figure_mode

fma, plt = loading_figure_mode(develop=False)

from mcmc.mode_simulation import ModeSimulation
from isingmodel.ising_model import IsingModel

if __name__ == "__main__":
    import os

    dir_path = os.path.dirname(os.path.realpath(__file__))
    os.chdir(dir_path)

    # Overwrite data directory?
    ising_model = IsingModel(beta=0.1, J=1.0, h=0.0, dimensions=[4, 4])

    from mcmctools.utils.utils import linspace_rp_intervals
    rp_intervals = linspace_rp_intervals(0.4, 0.6, 2)

    mode_simulation = ModeSimulation(model=ising_model,
                                     sim_base_dir="./",
                                     rel_data_path="./data/Test/",
                                     rel_results_path="./data/Test/results/",
                                     running_parameter_kind="systembase_params", running_parameter="beta",
                                     rp_intervals=rp_intervals
                                     )

    mode_simulation.equilibrium_time_simulation(run=True, eval=True, sample_size=20, number_of_steps=5000,
                                                eval_confidence_range=0.1,
                                                eval_confidence_window=100, measure="Mean")

    mode_simulation.compute_equilibrium_time(from_file=True,
                                             fma=fma)  # -> uses rel_data_path and rp_intervals??to load parameters

    mode_simulation.correlation_time_simulation(run=True, eval=False, minimum_sample_size=100,
                                                maximum_correlation_time=1000,
                                                equilibrium_time_rel_results_path="./data/Test/results/", measure="Mean"
                                                )

    mode_simulation.compute_correlation_time(from_file=True, fma=fma)

    mode_simulation.expectation_value_simulation(run=True, eval=False,
                                                 correlation_time_rel_results_path="./data/Test/results/",
                                                 number_of_measurements=10000,
                                                 equilibrium_time_rel_results_path="./data/Test/results/",
                                                 measures=["Mean", "Config"]
                                                 )

    mode_simulation.compute_expectation_value(from_file=True)  # measures=["Mean", "SecondMoment"])

    data, sim_params = mode_simulation.load_data_from_file()

    # mode_simulation.load_expectation_values_from_file()
    print(mode_simulation.expectation_values)

