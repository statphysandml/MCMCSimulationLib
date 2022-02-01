import numpy as np
import pandas as pd

from mcmctools.utils.utils import linspace_rp_intervals
from mcmctools.evaluation_module import EvaluationModule


if __name__ == '__main__':
    evaluation_module = EvaluationModule(sim_base_dir="../", rel_data_path="data/ScalarTheorySimulation/", rel_results_path="results/ScalarTheorySimulation/",
                                         running_parameter_kind=None,
                                         running_parameter="kappa",
                                         rp_values=linspace_rp_intervals(0.22, 0.3, 9))
    # evaluation_module.compute_equilibrium_time(from_file=True)

    evaluation_module.compute_expectation_value(from_file=True)